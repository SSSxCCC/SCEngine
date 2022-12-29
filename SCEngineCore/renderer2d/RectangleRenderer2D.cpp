#include "renderer2d/RectangleRenderer2D.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"
#include "core/Transform2D.h"
#include "asset/AssetManager.h"
#include "SCEngine.h"

void RectangleRenderer2D::onCreate() {
    mVulkanManager = mGameObject->mGameWorld->mSCEngine->mVulkanManager;

    // Create mDescriptorSetLayout
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    VkDescriptorSetLayoutCreateInfo layoutInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    if (vkCreateDescriptorSetLayout(mVulkanManager->getDevice(), &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // Create mPipelineLayout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional
    if (vkCreatePipelineLayout(mVulkanManager->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Create mGraphicsPipeline
    AssetManager* assetManager = mGameObject->mGameWorld->mSCEngine->mAssetManager;
    mGraphicsPipeline = mVulkanManager->createGraphicsPipeline(
            assetManager->readFile("SCEngineAsset/shader/vertShader_pvm_pos2d_opaqueColor.spv"),
            assetManager->readFile("SCEngineAsset/shader/fragShader_opaqueColor.spv"),
            Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), mPipelineLayout, mVulkanManager->getRenderPass());

    // Create mVertexBuffer, mIndexBuffer, mUniformBuffers
    mVulkanManager->createVertexBuffer(mVertices.data(), sizeof(mVertices[0]) * mVertices.size(), mVertexBuffer, mVertexBufferMemory);
    mVulkanManager->createIndexBuffer(mIndices.data(), sizeof(mIndices[0]) * mIndices.size(), mIndexBuffer, mIndexBufferMemory);
    mVulkanManager->createUniformBuffers(sizeof(glm::mat4), mUniformBuffers, mUniformBuffersMemory, mUniformBuffersMapped);

    // Create mDescriptorSets
    std::vector<VkDescriptorSetLayout> layouts(mVulkanManager->getMaxFrames(), mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfo.descriptorPool = mVulkanManager->getDescriptorPool();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(mVulkanManager->getMaxFrames());
    allocInfo.pSetLayouts = layouts.data();
    mDescriptorSets.resize(mVulkanManager->getMaxFrames());
    if (vkAllocateDescriptorSets(mVulkanManager->getDevice(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = mUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(mVulkanManager->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void RectangleRenderer2D::onDraw(const DrawData& drawData) {
    auto transform2D = mGameObject->getScript<Transform2D>();
    glm::mat4 model = transform2D ? transform2D->buildModelMatrix() : glm::mat4(1.0f);
    glm::mat4 pvm = drawData.projectionViewMatrix * model;
    memcpy(mUniformBuffersMapped[mVulkanManager->getCurrentFrame(drawData.forEditor)], &pvm, sizeof(pvm));

    vkCmdBindPipeline(drawData.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(drawData.extent.width);
    viewport.height = static_cast<float>(drawData.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(drawData.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = drawData.extent;
    vkCmdSetScissor(drawData.commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { mVertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(drawData.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(drawData.commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(drawData.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[mVulkanManager->getCurrentFrame(drawData.forEditor)], 0, nullptr);

    vkCmdDrawIndexed(drawData.commandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
}

void RectangleRenderer2D::onDestroy() {
    vkDeviceWaitIdle(mVulkanManager->getDevice());
    vkDestroyPipeline(mVulkanManager->getDevice(), mGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mVulkanManager->getDevice(), mPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(mVulkanManager->getDevice(), mDescriptorSetLayout, nullptr);
    vkDestroyBuffer(mVulkanManager->getDevice(), mIndexBuffer, nullptr);
    vkFreeMemory(mVulkanManager->getDevice(), mIndexBufferMemory, nullptr);
    vkDestroyBuffer(mVulkanManager->getDevice(), mVertexBuffer, nullptr);
    vkFreeMemory(mVulkanManager->getDevice(), mVertexBufferMemory, nullptr);
    for (size_t i = 0; i < mVulkanManager->getMaxFrames(); i++) {
        vkDestroyBuffer(mVulkanManager->getDevice(), mUniformBuffers[i], nullptr);
        vkFreeMemory(mVulkanManager->getDevice(), mUniformBuffersMemory[i], nullptr);
    }
}
