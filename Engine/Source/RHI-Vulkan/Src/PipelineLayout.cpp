//
// Created by Zach Lee on 2022/4/2.
//
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    VKPipelineLayout::VKPipelineLayout(VKDevice& dev, const PipelineLayoutCreateInfo& createInfo)
        : device(dev), PipelineLayout(createInfo)
    {
        CreateNativePipelineLayout(createInfo);
    }

    VKPipelineLayout::~VKPipelineLayout()
    {
        if (pipelineLayout) {
            vkDestroyPipelineLayout(device.GetVkDevice(), pipelineLayout, nullptr);
        }
    }

    void VKPipelineLayout::Destroy()
    {
        delete this;
    }

    VkPipelineLayout VKPipelineLayout::GetVkPipelineLayout() const
    {
        return pipelineLayout;
    }

    void VKPipelineLayout::CreateNativePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        std::vector<VkDescriptorSetLayout> setLayouts(createInfo.bindGroupLayoutNum);
        for (uint32_t i = 0; i < createInfo.bindGroupLayoutNum; ++i) {
            const auto* vulkanBindGroup = static_cast<const VKBindGroupLayout*>(createInfo.bindGroupLayouts[i]);
            setLayouts[i] = vulkanBindGroup->GetVkDescriptorSetLayout();
        }

        std::vector<VkPushConstantRange> pushConstants(createInfo.pipelineConstantLayoutNum);
        for (uint32_t i = 0; i < createInfo.pipelineConstantLayoutNum; ++i) {
            const auto& constantInfo = createInfo.pipelineConstantLayouts[i];
            pushConstants[i].stageFlags = FromRHI(constantInfo.stageFlags);
            pushConstants[i].offset = constantInfo.offset;
            pushConstants[i].size = constantInfo.size;
        }

        VkPipelineLayoutCreateInfo plInfo= {};
        plInfo.pSetLayouts = setLayouts.data();
        plInfo.pPushConstantRanges = pushConstants.data();
        Assert(vkCreatePipelineLayout(device.GetVkDevice(), &plInfo, nullptr, &pipelineLayout) == VK_SUCCESS);
    }

}