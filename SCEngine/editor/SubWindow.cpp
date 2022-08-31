#include "editor/SubWindow.h"
#include "glad/gl.h"
#include "imgui/imgui.h"
#include <iostream>

SubWindow::SubWindow() {
	mFocus = false;
	// width and height will be set to ImGui window size
	mWidth = 1;
	mHeight = 1;

	glGenFramebuffers(1, &mFbo);
	updateFrameBuffer();
}

SubWindow::~SubWindow() {
	glDeleteTextures(1, &mTexture);
	glDeleteRenderbuffers(1, &mRbo);
	glDeleteFramebuffers(1, &mFbo);
}

void SubWindow::update() {
	ImGui::Begin("editor");
	mFocus = ImGui::IsWindowFocused();
	ImGui::Image((ImTextureID)mTexture, ImVec2(mWidth, mHeight), ImVec2(0, 1.0f), ImVec2(1.0f, 0));
	ImVec2 windowSize = ImGui::GetWindowSize();
	windowSize.y = std::max(windowSize.y - 50.0f, 1.0f);
	if (mWidth != windowSize.x || mHeight != windowSize.y) {
		mWidth = windowSize.x;
		mHeight = windowSize.y;

		glDeleteTextures(1, &mTexture);
		glDeleteRenderbuffers(1, &mRbo);
		updateFrameBuffer();
	}
	ImGui::End();
}

void SubWindow::updateFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

	glGenRenderbuffers(1, &mRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
