#pragma once

// For used by Hammer application
#include "Hammer/Core/Base.h"
#include "Hammer/Core/Application.h"
#include "Hammer/Core/Layer.h"
#include "Hammer/Core/Log.h"
#include "Hammer/Core/Assert.h"
#include "Hammer/Core/Timestep.h"
#include "Hammer/Renderer/OrthographicCameraController.h"

//----Input System--------------------
#include "Hammer/Core/Input.h"
#include "Hammer/Core/KeyCode.h"
#include "Hammer/Core/MouseCode.h"
//------------------------------------

//----ECS-----------------------------
#include "Hammer/Scene/Scene.h"
#include "Hammer/Scene/Components.h"
#include "Hammer/Scene/Entity.h"
#include "Hammer/Scene/ScriptableEntity.h"
//------------------------------------

//----GUI-----------------------------
#include "Hammer/ImGui/ImGuiLayer.h"
//------------------------------------

//----Render--------------------------
#include "Hammer/Renderer/Renderer.h"
#include "Hammer/Renderer/Renderer2D.h"
#include "Hammer/Renderer/RenderCommand.h"

#include "Hammer/Renderer/Buffer.h"
#include "Hammer/Renderer/Shader.h"
#include "Hammer/Renderer/VertexArray.h"
#include "Hammer/Renderer/Framebuffer.h"
#include "Hammer/Renderer/Texture.h"
#include "Hammer/Renderer/SubTexture2D.h"
#include "Hammer/Renderer/OrthographicCamera.h"
//------------------------------------


//----Entry Point---------------------
//#include "Hammer/Core/EntryPoint.h"
//------------------------------------