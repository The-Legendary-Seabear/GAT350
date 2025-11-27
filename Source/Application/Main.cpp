
int main(int argc, char* argv[]) {
    neu::file::SetCurrentDirectory("Assets");
    LOG_INFO("current directory {}", neu::file::GetCurrentDirectory());

    // initialize engine
    LOG_INFO("initialize engine...");
    neu::GetEngine().Initialize();

    SDL_Event e;
    bool quit = false;

    //create render textures
    auto renderTexture = std::make_shared<neu::RenderTexture>();
    renderTexture->Create(512, 512);
    neu::Resources().AddResource("renderTexture", renderTexture);

    renderTexture = std::make_shared<neu::RenderTexture>();
    renderTexture->Create(1024, 1024);
    neu::Resources().AddResource("postprocessTexture", renderTexture);

    renderTexture = std::make_shared<neu::RenderTexture>();
    renderTexture->CreateDepth(1024, 1024);
    neu::Resources().AddResource("shadowTexture", renderTexture);

    // initialize scene
    auto scene = std::make_unique<neu::Scene>();
    scene->Load("scenes/scene04.json");
    scene->Start();

    auto editor = std::make_unique<neu::Editor>();

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

        scene->Update(dt);

        editor->Begin();
        editor->UpdateGui(*scene);
        
        // draw

        scene->Draw(neu::GetEngine().GetRenderer());

        // draw ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        neu::GetEngine().GetRenderer().Present();
    }

    neu::GetEngine().Shutdown();

    return 0;
}
