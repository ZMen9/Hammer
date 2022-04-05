-- Hammer Dependencies
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Hammer/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hammer/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Hammer/vendor/imgui/"
IncludeDir["glm"] = "%{wks.location}/Hammer/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hammer/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hammer/vendor/entt/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hammer/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Hammer/vendor/Box2D/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hammer/vendor/yaml-cpp/include"
IncludeDir["shaderc"] = "%{wks.location}/Hammer/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Hammer/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Hammer/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/Hammer/vendor/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"