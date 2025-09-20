//由于一些原因，CatTuber必须改动SDL的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。

//此文件修改static D3D12Texture *D3D12_INTERNAL_CreateTexture函数 有两个修改部分



static D3D12Texture* D3D12_INTERNAL_CreateTexture(
    D3D12Renderer* renderer,
    const SDL_GPUTextureCreateInfo* createinfo,
    bool isSwapchainTexture,
    const char* debugName)
{
    D3D12Texture* texture;
    ID3D12Resource* handle;
    D3D12_HEAP_PROPERTIES heapProperties;
    D3D12_HEAP_FLAGS heapFlags = (D3D12_HEAP_FLAGS)0;
    D3D12_RESOURCE_DESC desc;
    D3D12_RESOURCE_FLAGS resourceFlags = (D3D12_RESOURCE_FLAGS)0;
    D3D12_RESOURCE_STATES initialState = (D3D12_RESOURCE_STATES)0;
    D3D12_CLEAR_VALUE clearValue;
    DXGI_FORMAT format;
    bool useClearValue = false;
    bool needsSRV =
        (createinfo->usage & SDL_GPU_TEXTUREUSAGE_SAMPLER) ||
        (createinfo->usage & SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ) ||
        (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_READ);
    bool needsUAV =
        (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE) ||
        (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE);
    HRESULT res;

    texture = (D3D12Texture*)SDL_calloc(1, sizeof(D3D12Texture));
    if (!texture) {
        return NULL;
    }

    Uint32 layerCount = createinfo->type == SDL_GPU_TEXTURETYPE_3D ? 1 : createinfo->layer_count_or_depth;
    Uint32 depth = createinfo->type == SDL_GPU_TEXTURETYPE_3D ? createinfo->layer_count_or_depth : 1;
    bool isMultisample = createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1;

    format = SDLToD3D12_TextureFormat[createinfo->format];

    if (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COLOR_TARGET) {
        resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        useClearValue = true;
        clearValue.Format = format;
        clearValue.Color[0] = SDL_GetFloatProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT, 0);
        clearValue.Color[1] = SDL_GetFloatProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT, 0);
        clearValue.Color[2] = SDL_GetFloatProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT, 0);
        clearValue.Color[3] = SDL_GetFloatProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT, 0);
    }

    if (createinfo->usage & SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET) {
        resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        useClearValue = true;
        clearValue.Format = SDLToD3D12_DepthFormat[createinfo->format];
        clearValue.DepthStencil.Depth = SDL_GetFloatProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_DEPTH_FLOAT, 0);
        clearValue.DepthStencil.Stencil = (UINT8)SDL_GetNumberProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_STENCIL_NUMBER, 0);
        format = needsSRV ? SDLToD3D12_TypelessFormat[createinfo->format] : SDLToD3D12_DepthFormat[createinfo->format];
    }

    if (needsUAV) {
        resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 0; // We don't do multi-adapter operation
    heapProperties.VisibleNodeMask = 0;  // We don't do multi-adapter operation

    heapFlags = isSwapchainTexture ? D3D12_HEAP_FLAG_ALLOW_DISPLAY : D3D12_HEAP_FLAG_NONE;

    //修改部分1
    //修改部分1
    //修改部分1
    //修改部分1
    //修改部分1:根据需要，为heapFlags添加新的内容
    bool isShare=SDL_GetBooleanProperty(createinfo->props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_BOOL,false);
    if (isShare)
    {
        heapFlags = heapFlags | D3D12_HEAP_FLAG_SHARED;
    }

    //修改部分1
    //修改部分1
    //修改部分1
    //修改部分1

    if (createinfo->type != SDL_GPU_TEXTURETYPE_3D) {
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = isSwapchainTexture ? 0 : isMultisample ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Width = createinfo->width;
        desc.Height = createinfo->height;
        desc.DepthOrArraySize = (UINT16)createinfo->layer_count_or_depth;
        desc.MipLevels = (UINT16)createinfo->num_levels;
        desc.Format = format;
        desc.SampleDesc.Count = SDLToD3D12_SampleCount[createinfo->sample_count];
        desc.SampleDesc.Quality = isMultisample ? D3D12_STANDARD_MULTISAMPLE_PATTERN : 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // Apparently this is the most efficient choice
        desc.Flags = resourceFlags;
    }
    else {
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Width = createinfo->width;
        desc.Height = createinfo->height;
        desc.DepthOrArraySize = (UINT16)createinfo->layer_count_or_depth;
        desc.MipLevels = (UINT16)createinfo->num_levels;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = resourceFlags;
    }

    initialState = isSwapchainTexture ? D3D12_RESOURCE_STATE_PRESENT : D3D12_INTERNAL_DefaultTextureResourceState(createinfo->usage);

    res = ID3D12Device_CreateCommittedResource(
        renderer->device,
        &heapProperties,
        heapFlags,
        &desc,
        initialState,
        useClearValue ? &clearValue : NULL,
        D3D_GUID(D3D_IID_ID3D12Resource),
        (void**)&handle);
    if (FAILED(res)) {
        D3D12_INTERNAL_SetError(renderer, "Failed to create texture!", res);
        D3D12_INTERNAL_DestroyTexture(renderer, texture);
        return NULL;
    }

    texture->resource = handle;

    // Create the SRV if applicable
    if (needsSRV) {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

        D3D12_INTERNAL_AssignStagingDescriptorHandle(
            renderer,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            &texture->srvHandle);

        srvDesc.Format = SDLToD3D12_TextureFormat[createinfo->format];
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (createinfo->type == SDL_GPU_TEXTURETYPE_CUBE) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = createinfo->num_levels;
            srvDesc.TextureCube.MostDetailedMip = 0;
            srvDesc.TextureCube.ResourceMinLODClamp = 0;
        }
        else if (createinfo->type == SDL_GPU_TEXTURETYPE_CUBE_ARRAY) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            srvDesc.TextureCubeArray.MipLevels = createinfo->num_levels;
            srvDesc.TextureCubeArray.MostDetailedMip = 0;
            srvDesc.TextureCubeArray.First2DArrayFace = 0;
            srvDesc.TextureCubeArray.NumCubes = createinfo->layer_count_or_depth / 6;
            srvDesc.TextureCubeArray.ResourceMinLODClamp = 0;
        }
        else if (createinfo->type == SDL_GPU_TEXTURETYPE_2D_ARRAY) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MipLevels = createinfo->num_levels;
            srvDesc.Texture2DArray.MostDetailedMip = 0;
            srvDesc.Texture2DArray.FirstArraySlice = 0;
            srvDesc.Texture2DArray.ArraySize = layerCount;
            srvDesc.Texture2DArray.ResourceMinLODClamp = 0;
            srvDesc.Texture2DArray.PlaneSlice = 0;
        }
        else if (createinfo->type == SDL_GPU_TEXTURETYPE_3D) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MipLevels = createinfo->num_levels;
            srvDesc.Texture3D.MostDetailedMip = 0;
            srvDesc.Texture3D.ResourceMinLODClamp = 0; // default behavior
        }
        else {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = createinfo->num_levels;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.PlaneSlice = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = 0; // default behavior
        }

        ID3D12Device_CreateShaderResourceView(
            renderer->device,
            handle,
            &srvDesc,
            texture->srvHandle.cpuHandle);
    }

    SDL_SetAtomicInt(&texture->referenceCount, 0);

    texture->subresourceCount = createinfo->num_levels * layerCount;
    texture->subresources = (D3D12TextureSubresource*)SDL_calloc(
        texture->subresourceCount, sizeof(D3D12TextureSubresource));
    if (!texture->subresources) {
        D3D12_INTERNAL_DestroyTexture(renderer, texture);
        return NULL;
    }
    for (Uint32 layerIndex = 0; layerIndex < layerCount; layerIndex += 1) {
        for (Uint32 levelIndex = 0; levelIndex < createinfo->num_levels; levelIndex += 1) {
            Uint32 subresourceIndex = D3D12_INTERNAL_CalcSubresource(
                levelIndex,
                layerIndex,
                createinfo->num_levels);

            texture->subresources[subresourceIndex].parent = texture;
            texture->subresources[subresourceIndex].layer = layerIndex;
            texture->subresources[subresourceIndex].level = levelIndex;
            texture->subresources[subresourceIndex].depth = depth;
            texture->subresources[subresourceIndex].index = subresourceIndex;

            texture->subresources[subresourceIndex].rtvHandles = NULL;
            texture->subresources[subresourceIndex].uavHandle.heap = NULL;
            texture->subresources[subresourceIndex].dsvHandle.heap = NULL;

            // Create RTV if needed
            if (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COLOR_TARGET) {
                texture->subresources[subresourceIndex].rtvHandles = (D3D12StagingDescriptor*)SDL_calloc(depth, sizeof(D3D12StagingDescriptor));

                for (Uint32 depthIndex = 0; depthIndex < depth; depthIndex += 1) {
                    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;

                    D3D12_INTERNAL_AssignStagingDescriptorHandle(
                        renderer,
                        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                        &texture->subresources[subresourceIndex].rtvHandles[depthIndex]);

                    rtvDesc.Format = SDLToD3D12_TextureFormat[createinfo->format];

                    if (createinfo->type == SDL_GPU_TEXTURETYPE_2D_ARRAY || createinfo->type == SDL_GPU_TEXTURETYPE_CUBE || createinfo->type == SDL_GPU_TEXTURETYPE_CUBE_ARRAY) {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtvDesc.Texture2DArray.MipSlice = levelIndex;
                        rtvDesc.Texture2DArray.FirstArraySlice = layerIndex;
                        rtvDesc.Texture2DArray.ArraySize = 1;
                        rtvDesc.Texture2DArray.PlaneSlice = 0;
                    }
                    else if (createinfo->type == SDL_GPU_TEXTURETYPE_3D) {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                        rtvDesc.Texture3D.MipSlice = levelIndex;
                        rtvDesc.Texture3D.FirstWSlice = depthIndex;
                        rtvDesc.Texture3D.WSize = 1;
                    }
                    else if (isMultisample) {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        rtvDesc.Texture2D.MipSlice = levelIndex;
                        rtvDesc.Texture2D.PlaneSlice = 0;
                    }

                    ID3D12Device_CreateRenderTargetView(
                        renderer->device,
                        texture->resource,
                        &rtvDesc,
                        texture->subresources[subresourceIndex].rtvHandles[depthIndex].cpuHandle);
                }
            }

            // Create DSV if needed
            if (createinfo->usage & SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET) {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;

                D3D12_INTERNAL_AssignStagingDescriptorHandle(
                    renderer,
                    D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                    &texture->subresources[subresourceIndex].dsvHandle);

                dsvDesc.Format = SDLToD3D12_DepthFormat[createinfo->format];
                dsvDesc.Flags = (D3D12_DSV_FLAGS)0;

                if (isMultisample) {
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
                }
                else {
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    dsvDesc.Texture2D.MipSlice = levelIndex;
                }

                ID3D12Device_CreateDepthStencilView(
                    renderer->device,
                    texture->resource,
                    &dsvDesc,
                    texture->subresources[subresourceIndex].dsvHandle.cpuHandle);
            }

            // Create subresource UAV if necessary
            if (needsUAV) {
                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;

                D3D12_INTERNAL_AssignStagingDescriptorHandle(
                    renderer,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                    &texture->subresources[subresourceIndex].uavHandle);

                uavDesc.Format = SDLToD3D12_TextureFormat[createinfo->format];

                if (createinfo->type == SDL_GPU_TEXTURETYPE_2D_ARRAY || createinfo->type == SDL_GPU_TEXTURETYPE_CUBE || createinfo->type == SDL_GPU_TEXTURETYPE_CUBE_ARRAY) {
                    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                    uavDesc.Texture2DArray.MipSlice = levelIndex;
                    uavDesc.Texture2DArray.FirstArraySlice = layerIndex;
                    uavDesc.Texture2DArray.ArraySize = 1;
                }
                else if (createinfo->type == SDL_GPU_TEXTURETYPE_3D) {
                    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                    uavDesc.Texture3D.MipSlice = levelIndex;
                    uavDesc.Texture3D.FirstWSlice = 0;
                    uavDesc.Texture3D.WSize = depth;
                }
                else {
                    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                    uavDesc.Texture2D.MipSlice = levelIndex;
                    uavDesc.Texture2D.PlaneSlice = 0;
                }

                ID3D12Device_CreateUnorderedAccessView(
                    renderer->device,
                    texture->resource,
                    NULL,
                    &uavDesc,
                    texture->subresources[subresourceIndex].uavHandle.cpuHandle);
            }
        }
    }

    D3D12_INTERNAL_SetResourceName(
        renderer,
        texture->resource,
        debugName);


    //修改部分2
    //修改部分2
    //修改部分2
    //修改部分2
    //修改部分2
    //修改部分2:传回share handle
    if (isShare)
    {
        HANDLE* pShareHandle= SDL_GetPointerProperty(createinfo->props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_HANDLE_POINTER, 0);
        if (pShareHandle)
        {
            ID3D12Device_CreateSharedHandle(renderer->device, texture->resource,NULL, GENERIC_ALL, NULL, pShareHandle);
        }
    }

    //修改部分2
    //修改部分2
    //修改部分2
    //修改部分2
    //修改部分2


    return texture;
}