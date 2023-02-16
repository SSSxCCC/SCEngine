#include "sc/editor/DebugDrawPhysics2D.h"
#include "sc/core/Engine.h"
#include "sc/core/Transform2D.h"
#include "sc/graphics/VulkanManager.h"
#include "sc/graphics/DrawData.h"
#include "sc/asset/AssetManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include <array>

namespace sc {

class RenderBase {
public:
    struct Vertex {
        b2Vec2 position;
        b2Color color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);
            return attributeDescriptions;
        }
    };

    RenderBase(VulkanManager* vulkanManager) : mVulkanManager(vulkanManager) { }

    virtual ~RenderBase() { }

    void setDrawData(const DrawData& drawData) {
        mDrawData = drawData;
    }

    void flush() {
        if (mCount == 0) {
            return;
        }
        onFlush();
        mCount = 0;
    }

protected:
    VulkanManager* const mVulkanManager;
    DrawData mDrawData;
    int mCount = 0;
    
    virtual void onFlush() = 0;
};


class RenderPoints : public RenderBase {
public:
    struct Vertex {
        b2Vec2 position;
        b2Color color;
        float size;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, size);
            return attributeDescriptions;
        }
    };

    RenderPoints(VulkanManager* vulkanManager, AssetManager* assetManager) : RenderBase(vulkanManager) {
        // Create pushConstant
        VkPushConstantRange pushConstant{};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);

        // Create mPipelineLayout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
        if (vkCreatePipelineLayout(mVulkanManager->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // Create mGraphicsPipeline
        mGraphicsPipeline = mVulkanManager->createGraphicsPipeline(
            assetManager->readFile("SCEngineAssets/Shaders/vertShader_pvm_pos2d_color_size.spv"),
            assetManager->readFile("SCEngineAssets/Shaders/fragShader_color.spv"),
            Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), mPipelineLayout, mVulkanManager->getRenderPass(), VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

        // Create mVertexBuffers
        mVertexBuffers.resize(mVulkanManager->getMaxFrames());
        mVertexBufferMemory.resize(mVulkanManager->getMaxFrames());
        mVertexBuffersMapped.resize(mVulkanManager->getMaxFrames());
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            mVulkanManager->createBuffer(mVertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mVertexBuffers[i], mVertexBufferMemory[i]);
            vkMapMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], 0, mVertices.size() * sizeof(Vertex), 0, &mVertexBuffersMapped[i]);
        }
    }

    ~RenderPoints() {
        vkDeviceWaitIdle(mVulkanManager->getDevice());
        vkDestroyPipeline(mVulkanManager->getDevice(), mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(mVulkanManager->getDevice(), mPipelineLayout, nullptr);
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            vkDestroyBuffer(mVulkanManager->getDevice(), mVertexBuffers[i], nullptr);
            vkFreeMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], nullptr);
        }
    }

    void add(const Vertex&& v) {
        if (mCount == mVertices.size()) {
            flush();
        }
        mVertices[mCount++] = v;
    }

protected:
    void onFlush() override {
        memcpy(mVertexBuffersMapped[mVulkanManager->getCurrentFrame(mDrawData.forEditor)], mVertices.data(), mCount * sizeof(Vertex));

        vkCmdBindPipeline(mDrawData.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(mDrawData.extent.width);
        viewport.height = static_cast<float>(mDrawData.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mDrawData.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = mDrawData.extent;
        vkCmdSetScissor(mDrawData.commandBuffer, 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(mDrawData.commandBuffer, 0, 1, &(mVertexBuffers[mVulkanManager->getCurrentFrame(mDrawData.forEditor)]), offsets);
        vkCmdPushConstants(mDrawData.commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mDrawData.projectionViewMatrix);

        vkCmdDraw(mDrawData.commandBuffer, mCount, 1, 0, 0);
        //std::cout << "Points " << mCount << std::endl;
    }

private:
    std::array<Vertex, 512> mVertices;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;

    std::vector<VkBuffer> mVertexBuffers;
    std::vector<VkDeviceMemory> mVertexBufferMemory;
    std::vector<void*> mVertexBuffersMapped;
};


class RenderLines : public RenderBase {
public:
    RenderLines(VulkanManager* vulkanManager, AssetManager* assetManager) : RenderBase(vulkanManager) {
        // Create pushConstant
        VkPushConstantRange pushConstant{};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);

        // Create mPipelineLayout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
        if (vkCreatePipelineLayout(mVulkanManager->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // Create mGraphicsPipeline
        mGraphicsPipeline = mVulkanManager->createGraphicsPipeline(
            assetManager->readFile("SCEngineAssets/Shaders/vertShader_pvm_pos2d_color.spv"),
            assetManager->readFile("SCEngineAssets/Shaders/fragShader_color.spv"),
            Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), mPipelineLayout, mVulkanManager->getRenderPass(), VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

        // Create mVertexBuffers
        mVertexBuffers.resize(mVulkanManager->getMaxFrames());
        mVertexBufferMemory.resize(mVulkanManager->getMaxFrames());
        mVertexBuffersMapped.resize(mVulkanManager->getMaxFrames());
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            mVulkanManager->createBuffer(mVertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mVertexBuffers[i], mVertexBufferMemory[i]);
            vkMapMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], 0, mVertices.size() * sizeof(Vertex), 0, &mVertexBuffersMapped[i]);
        }
    }

    ~RenderLines() {
        vkDeviceWaitIdle(mVulkanManager->getDevice());
        vkDestroyPipeline(mVulkanManager->getDevice(), mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(mVulkanManager->getDevice(), mPipelineLayout, nullptr);
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            vkDestroyBuffer(mVulkanManager->getDevice(), mVertexBuffers[i], nullptr);
            vkFreeMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], nullptr);
        }
    }

    void add(const Vertex&& v1, const Vertex&& v2) {
        if (mCount == mVertices.size()) {
            flush();
        }
        mVertices[mCount++] = v1;
        mVertices[mCount++] = v2;
    }

protected:
    void onFlush() override {
        memcpy(mVertexBuffersMapped[mVulkanManager->getCurrentFrame(mDrawData.forEditor)], mVertices.data(), mCount * sizeof(Vertex));

        vkCmdBindPipeline(mDrawData.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(mDrawData.extent.width);
        viewport.height = static_cast<float>(mDrawData.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mDrawData.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = mDrawData.extent;
        vkCmdSetScissor(mDrawData.commandBuffer, 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(mDrawData.commandBuffer, 0, 1, &(mVertexBuffers[mVulkanManager->getCurrentFrame(mDrawData.forEditor)]), offsets);
        vkCmdPushConstants(mDrawData.commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mDrawData.projectionViewMatrix);

        vkCmdDraw(mDrawData.commandBuffer, mCount, 1, 0, 0);
        //std::cout << "Lines " << mCount << std::endl;
    }

private:
    std::array<Vertex, 512 * 2> mVertices;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;

    std::vector<VkBuffer> mVertexBuffers;
    std::vector<VkDeviceMemory> mVertexBufferMemory;
    std::vector<void*> mVertexBuffersMapped;
};


class RenderTriangles : public RenderBase {
public:
    RenderTriangles(VulkanManager* vulkanManager, AssetManager* assetManager) : RenderBase(vulkanManager) {
        // Create pushConstant
        VkPushConstantRange pushConstant{};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);

        // Create mPipelineLayout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
        if (vkCreatePipelineLayout(mVulkanManager->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // Create mGraphicsPipeline
        mGraphicsPipeline = mVulkanManager->createGraphicsPipeline(
            assetManager->readFile("SCEngineAssets/Shaders/vertShader_pvm_pos2d_color.spv"),
            assetManager->readFile("SCEngineAssets/Shaders/fragShader_color.spv"),
            Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), mPipelineLayout, mVulkanManager->getRenderPass());

        // Create mVertexBuffers
        mVertexBuffers.resize(mVulkanManager->getMaxFrames());
        mVertexBufferMemory.resize(mVulkanManager->getMaxFrames());
        mVertexBuffersMapped.resize(mVulkanManager->getMaxFrames());
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            mVulkanManager->createBuffer(mVertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mVertexBuffers[i], mVertexBufferMemory[i]);
            vkMapMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], 0, mVertices.size() * sizeof(Vertex), 0, &mVertexBuffersMapped[i]);
        }
    }

    ~RenderTriangles() {
        vkDeviceWaitIdle(mVulkanManager->getDevice());
        vkDestroyPipeline(mVulkanManager->getDevice(), mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(mVulkanManager->getDevice(), mPipelineLayout, nullptr);
        for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
            vkDestroyBuffer(mVulkanManager->getDevice(), mVertexBuffers[i], nullptr);
            vkFreeMemory(mVulkanManager->getDevice(), mVertexBufferMemory[i], nullptr);
        }
    }

    void add(const Vertex&& v1, const Vertex&& v2, const Vertex&& v3) {
        if (mCount == mVertices.size()) {
            flush();
        }
        mVertices[mCount++] = v1;
        mVertices[mCount++] = v2;
        mVertices[mCount++] = v3;
    }

protected:
    void onFlush() override {
        memcpy(mVertexBuffersMapped[mVulkanManager->getCurrentFrame(mDrawData.forEditor)], mVertices.data(), mCount * sizeof(Vertex));

        vkCmdBindPipeline(mDrawData.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(mDrawData.extent.width);
        viewport.height = static_cast<float>(mDrawData.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mDrawData.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = mDrawData.extent;
        vkCmdSetScissor(mDrawData.commandBuffer, 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(mDrawData.commandBuffer, 0, 1, &(mVertexBuffers[mVulkanManager->getCurrentFrame(mDrawData.forEditor)]), offsets);
        vkCmdPushConstants(mDrawData.commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mDrawData.projectionViewMatrix);

        vkCmdDraw(mDrawData.commandBuffer, mCount, 1, 0, 0);
        //std::cout << "Triangles " << mCount << std::endl;
    }

private:
    std::array<Vertex, 512 * 3> mVertices;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;

    std::vector<VkBuffer> mVertexBuffers;
    std::vector<VkDeviceMemory> mVertexBufferMemory;
    std::vector<void*> mVertexBuffersMapped;
};


// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw {
public:
	DebugDraw(VulkanManager* vulkanManager, AssetManager* assetManager) : mPoints(vulkanManager, assetManager), mLines(vulkanManager, assetManager), mTriangles(vulkanManager, assetManager) { }

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i) {
			b2Vec2 p2 = vertices[i];
            mLines.add({ p1, color }, { p2, color });
			p1 = p2;
		}
	}

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

		for (int32 i = 1; i < vertexCount - 1; ++i) {
            mTriangles.add({ vertices[0], fillColor }, { vertices[i], fillColor }, { vertices[i + 1], fillColor });
		}

		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i) {
			b2Vec2 p2 = vertices[i];
			mLines.add({ p1, color }, { p2, color });
			p1 = p2;
		}
	}

	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i) {
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
            mLines.add({ v1, color }, { v2, color });
			r1 = r2;
			v1 = v2;
		}
	}

	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 v0 = center;
		b2Vec2 r1(cosInc, sinInc);
		b2Vec2 v1 = center + radius * r1;
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		for (int32 i = 0; i < k_segments; ++i) {
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
            mTriangles.add({ v0, fillColor }, { v1, fillColor }, { v2, fillColor });
			r1 = r2;
			v1 = v2;
		}

		r1.Set(1.0f, 0.0f);
		v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i) {
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
            mLines.add({ v1, color }, { v2, color });
			r1 = r2;
			v1 = v2;
		}

		// Draw a line fixed in the circle to animate rotation.
		b2Vec2 p = center + radius * axis;
        mLines.add({ center, color }, { p, color });
	}

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {
        mLines.add({ p1, color }, { p2, color });
	}

	void DrawTransform(const b2Transform& xf) override {
		const float k_axisScale = 0.4f;
		b2Color red(1.0f, 0.0f, 0.0f);
		b2Color green(0.0f, 1.0f, 0.0f);
		b2Vec2 p1 = xf.p, p2;

		p2 = p1 + k_axisScale * xf.q.GetXAxis();
        mLines.add({ p1, red }, { p2, red });

		p2 = p1 + k_axisScale * xf.q.GetYAxis();
        mLines.add({ p1, green }, { p2, green });
	}

	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {
        mPoints.add({ p, color, size });
	}

	void DrawAABB(b2AABB* aabb, const b2Color& c) {
		b2Vec2 p1 = aabb->lowerBound;
		b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
		b2Vec2 p3 = aabb->upperBound;
		b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

        mLines.add({ p1, c }, { p2, c });
        mLines.add({ p2, c }, { p3, c });
        mLines.add({ p3, c }, { p4, c });
        mLines.add({ p4, c }, { p1, c });
	}

    void setDrawData(const DrawData& drawData) {
        mTriangles.setDrawData(drawData);
        mLines.setDrawData(drawData);
        mPoints.setDrawData(drawData);
    }

	void Flush() {
		mTriangles.flush();
		mLines.flush();
		mPoints.flush();
	}

private:
	RenderPoints mPoints;
	RenderLines mLines;
	RenderTriangles mTriangles;
};


void DebugDrawPhysics2D::onCreate() {
	mDebugDraw = new DebugDraw(mGameObject->mScene->mEngine->mVulkanManager, mGameObject->mScene->mEngine->mAssetManager);
	mPhysicsWorld2D = mGameObject->getScript<PhysicsWorld2D>();
	mPhysicsWorld2D->mWorld->SetDebugDraw(mDebugDraw);
}

void DebugDrawPhysics2D::onDraw(const DrawData& drawData) {
    if (drawData.forEditor) {
        mDebugDraw->setDrawData(drawData);
        mDebugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
        mPhysicsWorld2D->mWorld->DebugDraw();
        mDebugDraw->Flush();
    }
}

void DebugDrawPhysics2D::onDestroy() {
    if (mPhysicsWorld2D && mPhysicsWorld2D->mWorld) {
        mPhysicsWorld2D->mWorld->SetDebugDraw(nullptr);
    }
    mPhysicsWorld2D = nullptr;
    delete mDebugDraw;
    mDebugDraw = nullptr;
}

} // namespace sc
