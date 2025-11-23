#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include <list>
#include <memory>

namespace neu {
   
    class Actor;

   
    class Scene : public ISerializable, public GUI {
    public:
       
        Scene() = default;

        bool Start();

       
        void Destroyed();

       
        bool Load(const std::string& sceneName);

       
        void Read(const serial_data_t& value) override;

       
        void Update(float dt);

        void UpdateGui() override;

        void Draw(class Renderer& renderer);

        void DrawPass(class Renderer& renderer,
            std::vector<class Program*>& programs,
            std::vector<class LightComponent*>& lights,
            class CameraComponent* camera);

        void AddActor(std::unique_ptr<Actor> actor, bool start = true);

       
        void RemoveAllActors(bool force = false);

       
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        std::vector<T*> GetActorsOfType();

       
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        T* GetActorByName(const std::string& name);

       
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        std::vector<T*> GetActorsByTag(const std::string& tag);

        template<typename T>
        requires std::derived_from<T, Component>
        std::vector<T*> GetActorComponents();

    private:
            friend class Editor;
       
        std::list<std::unique_ptr<Actor>> m_actors;
        //float m_dt{ 0 };
        glm::vec3 m_ambientLight{ 0.2f, 0.2f, 0.2f };
        bool m_postprocess{ false };
    };

   
    template<typename T>
        requires std::derived_from<T, Actor>
    inline std::vector<T*> Scene::GetActorsOfType()
    {
        // Container to hold matching actors
        std::vector<T*> results;

        // Iterate through all actors in the scene
        for (auto& actor : m_actors) {
            // Attempt to cast the actor to the requested type
            // dynamic_cast returns nullptr if the cast fails (wrong type)
            T* object = dynamic_cast<T*>(actor.get());

            // Check if the cast was successful
            if (object) {
                // Cast succeeded - this actor is of type T
                // Add the raw pointer to results (scene retains ownership)
                results.push_back(object);
            }
        }

        // Return vector of all matching actors
        return results;
    }

   
    template<typename T>
        requires std::derived_from<T, Actor>
    inline T* Scene::GetActorByName(const std::string& name)
    {
        // Iterate through all actors looking for name match
        for (auto& actor : m_actors) {
            // Perform case-insensitive name comparison
            // equalsIgnoreCase is a utility function for string comparison
            if (neu::equalsIgnoreCase(actor->name, name)) {
                // Name matches - attempt to cast to the requested type
                T* object = dynamic_cast<T*>(actor.get());

                // Check if the cast was successful
                if (object) {
                    // Both name and type match - return this actor
                    return object;
                }
                // If cast failed, continue searching in case there's another
                // actor with the same name but different type
            }
        }

        // No matching actor found with correct name and type
        return nullptr;
    }

   
    template<typename T>
        requires std::derived_from<T, Actor>
    inline std::vector<T*> Scene::GetActorsByTag(const std::string& tag)
    {
        // Container to hold matching actors
        std::vector<T*> results;

        // Iterate through all actors in the scene
        for (auto& actor : m_actors) {
            // Check if actor's tag matches the requested tag (case-insensitive)
            // equalsIgnoreCase provides user-friendly tag matching
            if (neu::equalsIgnoreCase(actor->tag, tag)) {
                // Tag matches - attempt to cast to the requested type
                T* object = dynamic_cast<T*>(actor.get());

                // Check if the cast was successful
                if (object) {
                    // Both tag and type match - add to results
                    results.push_back(object);
                }
                // If cast fails, skip this actor (wrong type)
            }
        }

        // Return vector of all actors with matching tag and type
        return results;
    }

    template<typename T>
        requires std::derived_from<T, Component>
    inline std::vector<T*> Scene::GetActorComponents()
    {
        std::vector<T*> components;
        for (auto& actor : m_actors) {
            if (!actor->active) continue;

            auto component = actor->GetComponent<T>();
            if (component && component->active) {
                components.push_back(component);
            }
        }
        return components;
    }

    
}