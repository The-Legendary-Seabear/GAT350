#include "../Engine/Input/InputSystem.h"

int main(int argc, char* argv[]) {
    neu::file::SetCurrentDirectory("Assets");
    LOG_INFO("current directory {}", neu::file::GetCurrentDirectory());

    // initialize engine
    LOG_INFO("initialize engine...");
    neu::GetEngine().Initialize();

    // initialize scene
    SDL_Event e;
    bool quit = false;

    // OPENGL Initialization
    std::vector<neu::vec3> points{ { -0.5f, -0.5f, 0 }, { 0, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
    std::vector<neu::vec3> colors{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
    std::vector<neu::vec2> texcoord{ {0, 0}, {0.5f, 1}, {1, 1} };

    struct Vertex {
        neu::vec3 position;
        neu::vec3 color;
        neu::vec2 texcoord;
    };
    
    std::vector<Vertex> vertices{
        {{ -0.5f, -0.5f, 0 }, { 1, 0, 0 }, {0, 0}},
        {{ -0.5, 0.5f, 0 }, { 0, 1, 0 }, {0, 1}},
        {{ 0.5f, 0.5f, 0 }, { 0, 0, 1 }, {1, 1}},
        {{ 0.5f, -0.5f, 0 }, { 0, 0, 1 }, {1, 0}}
    };

    std::vector<short> indices{0, 1, 2, 2, 3, 0};

    auto model3d = std::make_shared<neu::Model>();
    model3d->Load("Models/spot.obj");

    //material
    auto material = neu::Resources().Get<neu::Material>("Materials/spot.mat");
    material->Bind();

    //lights
    material->program->SetUniform("u_ambient_light", glm::vec3{ 0.5f });
    neu::Transform light{ {2, 4, 0} };
    glm::vec3 lightColor{ 1 };

    //transform
    float rotation = 0;
    glm::vec3 eye{ 0, 0, 5 };

    neu::Transform transform{ {0, 0, 0} };
    neu::Transform camera{ {0, 0, 3} };
	//neu::Transform view{ {0, 0, 5} };

    //projection matrix
    float aspect = neu::GetEngine().GetRenderer().GetWidth() / (float)neu::GetEngine().GetRenderer().GetHeight();
    glm::mat4x4 projection = glm::perspective(glm::radians(90.0f), aspect, 0.01f, 100.0f);
    material->program->SetUniform("u_projection", projection);


    // MAIN LOOP
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            ImGui_ImplSDL3_ProcessEvent(&e);
        }

        // update
        neu::GetEngine().Update();
        float dt = neu::GetEngine().GetTime().GetDeltaTime();
        if (neu::GetEngine().GetInput().GetKeyPressed(SDL_SCANCODE_ESCAPE)) quit = true;

        rotation += dt * 90;

        //model matrix
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        //program->SetUniform("u_model", model);
        //transform.rotation.y += 90 * dt;
        material->program->SetUniform("u_model", transform.GetMatrix());
        float speed = 10.0f;

        //view matrix

        //if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_A)) eye.x += 10.0f * dt;
        //if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_D)) eye.x -= 10.0f * dt;
        //if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_W)) eye.z -= 10.0f * dt;
        //if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_S)) eye.z += 10.0f * dt;

        //float dt = neu::GetEngine().GetTime().GetDeltaTime();
        
        //rotation += 90 * dt;

        //    float speed = 10.0f;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_A)) camera.position.x -= speed * dt;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_D)) camera.position.x += speed * dt;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_Q)) camera.position.y -= speed * dt;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_E)) camera.position.y += speed * dt;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_W)) camera.position.z -= speed * dt;
        if (neu::GetEngine().GetInput().GetKeyDown(SDL_SCANCODE_S)) camera.position.z += speed * dt;
        // fill in the rest of the controls (WS and QE)

        //eye.y -= neu::GetEngine().GetInput().GetMouseDelta().y * 0.01f;
        glm::mat4 view = glm::lookAt(camera.position, camera.position + glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });
        material->program->SetUniform("u_view", view);

        material->program->SetUniform("u_light.color", lightColor);
        light.position.x = neu::math::sin(neu::GetEngine().GetTime().GetTime()) * 2;
        material->program->SetUniform("u_light.position", (glm::vec3)(view * glm::vec4(light.position, 1)));

        neu::GetEngine().GetRenderer().Clear();

        // start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        // set ImGui
        ImGui::Begin("Editor");
        //ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.1f);
		ImGui::ColorEdit3("Color", glm::value_ptr(lightColor)); 
        //light.UpdateGui();
        transform.UpdateGui();
        material->UpdateGui();
        ImGui::End();

        material->Bind();

        model3d->Draw(GL_TRIANGLES);
        // draw ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//vb->Draw(GL_TRIANGLES);
        //model3d->Draw(GL_TRIANGLES);


        neu::GetEngine().GetRenderer().Present();
    }

    neu::GetEngine().Shutdown();

    return 0;
}
