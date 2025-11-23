#include "Scene.h"
#include "Renderer/Renderer.h"

namespace neu {
    
    void Scene::Update(float dt) {
        // PHASE 1: Update all active actors
        // Loop through every actor in the scene container
        for (auto& actor : m_actors) {
            // Check the active flag before processing
            // This allows actors to be temporarily disabled without removal
            if (actor->active) {
                // Delegate to the actor's own Update implementation
                // dt allows for frame-rate independent movement and timing
                actor->Update(dt);
            }
        }

        // PHASE 2: Cleanup destroyed actors
        // Call Destroyed() on actors before removing them to allow cleanup
        // Then use std::erase_if for efficient removal (C++20 feature)
        std::erase_if(m_actors, [](auto& actor) {
            // Check if this actor should be removed
            if (actor->destroyed) {
                // Call Destroyed() to give the actor a chance to clean up
                // (release resources, notify other systems, etc.)
                actor->Destroyed();
                // Return true to remove this actor from the container
                return true;
            }
            // Actor is not destroyed, keep it in the scene
            return false;
            });
    }

    void Scene::UpdateGui()
    {
        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_ambientLight));
		ImGui::Checkbox("Post Process", &m_postprocess);
    }

   
    void Scene::Draw(Renderer& renderer) {
        //Get light
        auto lights = GetActorComponents<LightComponent>();


        //Get camera
        auto cameras = GetActorComponents<CameraComponent>();

        if (cameras.empty()) {
            LOG_WARNING("No camera active in scene");
                return;
        }

        // get programs
        std::set<Program*> programSet;
        for (auto& actor : m_actors) {
            ModelRenderer* model = actor->GetComponent<ModelRenderer>();
            if (!model || !model->active) continue;

            if (model->material && model->material->program) {
                programSet.insert(model->material->program.get());
            }
        }

        std::vector<Program*> programs(programSet.begin(), programSet.end());

        for (auto& camera : cameras) {
            PostProcessComponent* postprocessComponent = camera->owner->GetComponent<PostProcessComponent>();
            bool renderToTexture = camera->outputTexture && (!postprocessComponent || (postprocessComponent && m_postprocess));

            if (renderToTexture) {
                camera->outputTexture->BindFramebuffer();
                glViewport(0, 0, camera->outputTexture->m_size.x, camera->outputTexture->m_size.y);
            }
            camera->Clear();
            DrawPass(renderer, programs, lights, camera);
            if (renderToTexture) {
                camera->outputTexture->UnbindFramebuffer();
                glViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
            }

            if (renderToTexture && postprocessComponent) {
                auto postProcessProgram = Resources().Get<Program>("Shaders/postprocess.prog");
                postProcessProgram->Use();
				postprocessComponent->Apply(*postProcessProgram);
                camera->outputTexture->Bind();
                auto actor = GetActorByName("postprocess");
                actor->Draw(renderer);
            }
        }
    }

    void Scene::DrawPass(Renderer& renderer,
        std::vector<Program*>& programs,
        std::vector<LightComponent*>& lights,
        CameraComponent* camera)
    {
        //set shaders
        for (auto& program : programs) {
            program->Use();
            program->SetUniform("u_ambient_light", m_ambientLight);
            program->SetUniform("u_numLights", (int)lights.size());
            camera->SetProgram(*program);

            //set lights
            int index = 0;
            for (auto light : lights) {
                std::string lightName = "u_lights[" + std::to_string(index++) + "]";
                light->SetProgram(*program, lightName, camera->view);
            }
        }

        // Draw through all actors in the scene
        for (auto& actor : m_actors) {
            if (actor->active) {
                actor->Draw(renderer);
            }
        }
    }
   
    void Scene::AddActor(std::unique_ptr<Actor> actor, bool start) {
        // Validate that we're not trying to add a null pointer
        // ASSERT_MSG will help catch bugs during development
        ASSERT_MSG(actor, "Attempted to add null actor to scene");

        // Establish back-reference from actor to scene
        // This allows actors to query the scene, find other actors, etc.
        actor->scene = this;

        // Optionally initialize the actor immediately
        // During batch loading, we skip Start() and call it later for all actors
        if (start) actor->Start();

        // Transfer ownership to the scene's container
        // std::move is required to transfer unique_ptr ownership
        // push_back adds to the end of the list
        m_actors.push_back(std::move(actor));
    }

  
    void Scene::RemoveAllActors(bool force) {
        // Use manual iterator loop for conditional removal
        // std::erase_if can't be used here due to complex removal logic
        for (auto iter = m_actors.begin(); iter != m_actors.end(); ) {
            // Determine if this actor should be removed
            // Remove if: not persistent OR force removal is requested
            if (!(*iter)->persistent || force) {
                // Call Destroyed() on the actor before removing it
                // This allows the actor to clean up resources, save state, etc.
                (*iter)->Destroyed();

                // erase() invalidates current iterator but returns next valid iterator
                // This allows us to continue iteration safely
                iter = m_actors.erase(iter);
            }
            else {
                // This actor survives - manually advance to next
                // Don't use iter++ in the for loop due to conditional advancement
                iter++;
            }
        }
    }

    bool Scene::Start() {
        // Initialize all actors after the scene is fully constructed
        // This ensures all actors exist before any Start() methods run
        // allowing actors to safely find and reference other actors
        for (auto& actor : m_actors) {
            // Call each actor's initialization routine
            actor->Start();
        }

        // Return success - could be extended to handle initialization failures
        return true;
    }

    void Scene::Destroyed() {
        // Notify all actors that the scene is being destroyed
        // Gives actors a chance to clean up resources, save state, etc.
        for (auto& actor : m_actors) {
            actor->Destroyed();
        }

        // Clear the actor container
        // unique_ptr ensures all actors are properly deleted
        m_actors.clear();
    }

    
    bool Scene::Load(const std::string& sceneName) {
        // Create a document to hold the parsed serialized data
        neu::serial::document_t document;

        // Attempt to load and parse the scene file
        // Load() handles file I/O and JSON/serialization parsing
        if (!neu::serial::Load(sceneName, document)) {
            // Log error with scene name for debugging
            LOG_ERROR("Could not load scene {}", sceneName);
            return false; // Early return on failure
        }

        // Process the loaded document to populate the scene
        // Read() handles prototypes and actors sections
        Read(document);

        // Scene loaded successfully
        return true;
    }

   
    void Scene::Read(const serial_data_t& value) {
        // Load base Object properties first (name, active, etc.)
        // This calls the parent class's Read() implementation
        //Object::Read(value);
		SERIAL_READ_NAME(value, "ambient_light", m_ambientLight);
        SERIAL_READ_NAME(value, "postprocess", m_postprocess);
        // SECTION 1: Process prototype definitions
        // Check if the serialized data contains a "prototypes" section
        if (SERIAL_CONTAINS(value, prototypes)) {
            // Iterate through each prototype definition in the array
            for (auto& actorValue : SERIAL_AT(value, prototypes).GetArray()) {
                // Create a new base Actor instance via Factory
                // This uses the Factory pattern for type-safe object creation
                auto actor = Factory::Instance().Create<Actor>("Actor");

                // Load the actor's configuration from serialized data
                // This populates all actor properties (transform, components, etc.)
                actor->Read(actorValue);

                // Extract the actor's name to use as the prototype identifier
                std::string name = actor->name;

                // Register this configured actor as a reusable prototype
                // Other actors can now be instantiated from this template
                Factory::Instance().RegisterPrototype<Actor>(name, std::move(actor));
            }
        }

        // SECTION 2: Process direct actor definitions
        // Check if the serialized data contains an "actors" section
        if (SERIAL_CONTAINS(value, actors)) {
            // Iterate through each actor definition in the array
            for (auto& actorValue : SERIAL_AT(value, actors).GetArray()) {
                // Create a new Actor instance via Factory
                // Actors may reference prototypes defined above
                auto actor = Factory::Instance().Create<Actor>("Actor");

                // Load the actor's configuration from serialized data
                actor->Read(actorValue);

                // Add the actor to the scene without starting it yet
                // start=false defers initialization until all actors are loaded
                // This ensures all actors exist before any Start() methods run
                AddActor(std::move(actor), false);
            }
        }
    }
}