//
// Created by johnk on 31/12/2021.
//

#include <directx/d3dx12.h>

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/PhysicalDevice.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/DeviceMemory.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    DX12LogicalDevice::DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice, const LogicalDeviceCreateInfo* createInfo)
        : LogicalDevice(createInfo), instance(instance), property({})
    {
        CreateDevice(instance.GetDXGIFactory(), physicalDevice.GetDXGIAdapter());
        CreateCommandQueue(createInfo);
        ProcessExtensions(createInfo);
    }

    DX12LogicalDevice::~DX12LogicalDevice() = default;

    void DX12LogicalDevice::CreateDevice(ComPtr<IDXGIFactory4>& dxgiFactory, ComPtr<IDXGIAdapter1>& dxgiAdapter)
    {
        ThrowIfFailed(
            D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dx12Device)),
            "failed to create dx12 logical device"
        );
    }

    ComPtr<ID3D12Device>& DX12LogicalDevice::GetDX12Device()
    {
        return dx12Device;
    }

    void DX12LogicalDevice::CreateCommandQueue(const LogicalDeviceCreateInfo* createInfo)
    {
        for (size_t i = 0; i < createInfo->queueFamilyNum; i++) {
            auto& familyCreateInfo = createInfo->queueFamilyCreateInfos[i];
            auto& familyQueues = queueFamilies[familyCreateInfo.type];
            familyQueues.resize(familyCreateInfo.queueNum);

            QueueCreateInfo commandQueueCreateInfo {};
            commandQueueCreateInfo.type = familyCreateInfo.type;
            for (auto& familyQueue : familyQueues) {
                familyQueue = std::make_unique<DX12Queue>(*this, &commandQueueCreateInfo);
            }
        }
    }

    size_t DX12LogicalDevice::GetQueueNum(QueueFamilyType familyType)
    {
        auto iter = queueFamilies.find(familyType);
        if (iter == queueFamilies.end()) {
            return 0;
        }
        return iter->second.size();
    }

    Queue* DX12LogicalDevice::GetCommandQueue(QueueFamilyType familyType, size_t idx)
    {
        auto iter = queueFamilies.find(familyType);
        if (iter == queueFamilies.end()) {
            return nullptr;
        }
        if (idx < 0 || idx >= iter->second.size()) {
            return nullptr;
        }
        return iter->second[idx].get();
    }

    SwapChain* DX12LogicalDevice::CreateSwapChain(const SwapChainCreateInfo* createInfo)
    {
        return new DX12SwapChain(instance, createInfo);
    }

    void DX12LogicalDevice::DestroySwapChain(SwapChain* swapChain)
    {
        delete swapChain;
    }

    DX12LogicalDeviceProperty DX12LogicalDevice::GetProperty()
    {
        return property;
    }

    void DX12LogicalDevice::ProcessExtensions(const LogicalDeviceCreateInfo* createInfo)
    {
        static const std::unordered_map<std::string, DeviceExtProcessor> PROCESSORS = {
            { RHI_DEVICE_EXT_NAME_SWAP_CHAIN, [](DX12LogicalDeviceProperty& prop) -> void { prop.supportSwapChain = true; } }
        };

        for (size_t i = 0; i < createInfo->extensionNum; i++) {
            auto iter =  PROCESSORS.find(std::string(createInfo->extensions[i]));
            if (iter == PROCESSORS.end()) {
                continue;
            }
            iter->second(property);
        }
    }

    DeviceMemory* DX12LogicalDevice::AllocateDeviceMemory(const DeviceMemoryAllocateInfo* createInfo)
    {
        return new DX12DeviceMemory(*this, createInfo);
    }

    void DX12LogicalDevice::FreeDeviceMemory(DeviceMemory* deviceMemory)
    {
        delete deviceMemory;
    }

    Buffer* DX12LogicalDevice::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        return new DX12Buffer(createInfo);
    }

    void DX12LogicalDevice::DestroyBuffer(Buffer* buffer)
    {
        delete buffer;
    }

    void DX12LogicalDevice::BindBufferMemory(Buffer* buffer, DeviceMemory* deviceMemory)
    {
        auto dx12Buffer = static_cast<DX12Buffer*>(buffer);
        auto dx12DeviceMemory = static_cast<DX12DeviceMemory*>(deviceMemory);
        dx12Buffer->BindMemory(dx12DeviceMemory);
    }

    void* DX12LogicalDevice::MapDeviceMemory(DeviceMemory* deviceMemory)
    {
        auto dx12DeviceMemory = static_cast<DX12DeviceMemory*>(deviceMemory);
        auto range = CD3DX12_RANGE(0, 0);

        void* result;
        ThrowIfFailed(
            dx12DeviceMemory->GetDX12Resource()->Map(0, &range, &result),
            "failed to map dx12 resource"
        );
        return result;
    }

    void DX12LogicalDevice::UnmapDeviceMemory(DeviceMemory* deviceMemory)
    {
        auto dx12DeviceMemory = static_cast<DX12DeviceMemory*>(deviceMemory);
        dx12DeviceMemory->GetDX12Resource()->Unmap(0, nullptr);
    }
}
