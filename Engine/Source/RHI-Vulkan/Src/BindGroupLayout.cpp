//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <vector>

namespace RHI::Vulkan {
    VKBindGroupLayout::VKBindGroupLayout(VKDevice& dev, const BindGroupLayoutCreateInfo& createInfo)
        : BindGroupLayout(createInfo), device(dev)
    {
        CreateDescriptorSetLayout(createInfo);
    }

    VKBindGroupLayout::~VKBindGroupLayout()
    {
        if (setLayout) {
            vkDestroyDescriptorSetLayout(device.GetVkDevice(), setLayout, nullptr);
        }
    }

    void VKBindGroupLayout::Destroy()
    {
        delete this;
    }

    VkDescriptorSetLayout VKBindGroupLayout::GetVkDescriptorSetLayout() const
    {
        return setLayout;
    }

    void VKBindGroupLayout::CreateDescriptorSetLayout(const BindGroupLayoutCreateInfo& createInfo)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};

        std::vector<VkDescriptorSetLayoutBinding> bindings(createInfo.entryNum);
        for (size_t i = 0; i < createInfo.entryNum; ++i) {
            auto& entry = createInfo.entries[i];
            auto& binding = bindings[i];

            VkShaderStageFlags flags = FromRHI(entry.shaderVisibility);

            binding.descriptorType = VKEnumCast<BindingType, VkDescriptorType>(entry.binding.type);
            binding.descriptorCount = 1;
            binding.binding = entry.binding.platform.glsl.index;
            binding.stageFlags = flags;
        }

        layoutInfo.pBindings = bindings.data();

        Assert(vkCreateDescriptorSetLayout(device.GetVkDevice(), &layoutInfo, nullptr, &setLayout) == VK_SUCCESS);
    }

}