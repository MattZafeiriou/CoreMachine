#include <CoreEngine/CoreEngine.h>
#include <Shaders/Shader.h>
#include <Utils/EnvironmentVariablesUtils.cpp>
#include <CoreEngine/Scenes/Scene.h>
#include <CoreEngine/Framebuffers/Framebuffer.h>
#include <Window/CoreWindow.h>
#include <iostream>
#include <Objects/CoreObject.h>
#include <Objects/Cube/Cube.h>
#include <Objects/Plane/Plane.h>
#include <Objects/LightSources/Light.h>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

CoreEngine engine = CoreEngine(800, 600, 1);
Framebuffer framebuffer = Framebuffer(800, 600);
unsigned int textureColorbuffer = framebuffer.GetTexture();
void initimgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// i (as always) prefer the dark mode
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// initialize ImGui's glfw/opengl implementation 
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Render()
{
	// create imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// execute custom render function
	ImGui::NewFrame();
	ImGui::Begin("My Scene");

	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;

	engine.getCamera()->SetSize(window_width, window_height);

	framebuffer.Bind();
	framebuffer.Resize(window_width, window_height);
	framebuffer.Unbind();

	// we access the ImGui window size

	// we rescale the framebuffer to the actual window size here and reset the glViewport 
	glViewport(0, 0, window_width, window_height);

	// we get the screen position of the window
	ImVec2 pos = ImGui::GetCursorScreenPos();

	// and here we can add our created texture as image to ImGui
	// unfortunately we need to use the cast to void* or I didn't find another way tbh
	ImGui::GetWindowDrawList()->AddImage(
		(void*)textureColorbuffer,
		ImVec2(pos.x, pos.y),
		ImVec2(pos.x + window_width, pos.y + window_height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
	ImGui::End();
	ImGui::Render();

	framebuffer.Bind();
	engine.Render();
	framebuffer.Unbind();



	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup_current_context);
}

int main()
{
	Camera* camera = engine.getCamera();
	std::string userprofile = getEnvironmentVariable("USERPROFILE");
	std::string projectdir = "\\Program Files\\CoreMachine\\";
	std::string projectname = "Project1";
	setEnvironmentVariable("CORE_PROJECT_NAME", projectname.c_str());
	setEnvironmentVariable("CORE_PROJECT_DIR", (userprofile + projectdir + projectname).c_str());

	initimgui(engine.getWindow());
	engine.SetCustomRenderFunction(&Render);

	Shader* shader = new Shader("test.glsl", "test.glsl");
	engine.SetDefaultShader(shader);
	Shader* lightShader = new Shader("test.glsl", "test.glsl");
	engine.SetLightShader(lightShader);

	Scene* scene = new Scene(camera);
	engine.AddScene(scene);
	engine.SetCurrentScene(scene);

	Cube* bruh = new Cube(camera, shader, glm::vec4(1.0f));
	bruh->GetMaterials()[0]->SetDiffuse(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	scene->AddObject(bruh);

	Plane* plane = new Plane(camera, shader, glm::vec4(1.0f));
	plane->SetScale(50.0f);
	scene->AddObject(plane);

	Light* light = new Light(camera, "Models/Default/cube.obj", lightShader, glm::vec4(1.0f), 1.0f);
	light->SetPosition(1.0f, 1.0f, 1.0f);
	scene->AddLight(light);

	engine.Run();
	engine.Shutdown();
	return 0;
}