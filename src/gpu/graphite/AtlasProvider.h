/*
 * Copyright 2023 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef skgpu_graphite_AtlasProvider_DEFINED
#define skgpu_graphite_AtlasProvider_DEFINED

#include "include/core/SkColorType.h"
#include "include/core/SkRefCnt.h"
#include "include/private/base/SkTo.h"
#include "src/base/SkEnumBitMask.h"

#include <memory>
#include <unordered_map>

namespace skgpu::graphite {

class ComputePathAtlas;
class DrawContext;
class PathAtlas;
class RasterPathAtlas;
class Recorder;
class TextAtlasManager;
class TextureProxy;

/**
 * AtlasProvider groups various texture atlas management algorithms together.
 */
class AtlasProvider final {
public:
    explicit AtlasProvider(Recorder*);
    ~AtlasProvider() = default;

    // Returns the TextAtlasManager that provides access to persistent DrawAtlas'es used in glyph
    // rendering. This TextAtlasManager is always available.
    TextAtlasManager* textAtlasManager() const { return fTextAtlasManager.get(); }

    enum class PathAtlasFlags : unsigned {
        kNone    = 0b000,
        // ComputePathAtlas is supported
        kCompute = 0b001,
        // RasterPathAtlas is supported
        kRaster  = 0b010,
    };
    SK_DECL_BITMASK_OPS_FRIENDS(PathAtlasFlags);

    // Returns whether a particular atlas type is available
    bool isAvailable(PathAtlasFlags atlasType) {
        return SkToBool(fPathAtlasFlags & atlasType);
    }

    // Creates a new transient atlas handler that uses compute shaders to rasterize coverage masks
    // for path rendering. This method returns nullptr if compute shaders are not supported by the
    // owning Recorder's context.
    std::unique_ptr<ComputePathAtlas> createComputePathAtlas() const;

    // Gets the atlas handler that uses the CPU raster pipeline to create coverage masks
    // for path rendering.
    RasterPathAtlas* getRasterPathAtlas() const;

    // Return a TextureProxy with the given dimensions and color type.
    sk_sp<TextureProxy> getAtlasTexture(
            Recorder*, uint16_t width, uint16_t height, SkColorType, uint16_t identifier,
            bool requireStorageUsage);

    void clearTexturePool();

    // Push any pending uploads to atlases onto the draw context
    void recordUploads(DrawContext*, Recorder*);

private:
    std::unique_ptr<TextAtlasManager> fTextAtlasManager;

    // Accumulates atlas coverage masks generated by software rendering that are required by one or
    // more entries in `fPendingDraws`. During the snapUploadTask step, prior to pending draws
    // being snapped into a new DrawPass, any necessary uploads into an atlas texture are recorded
    // for the accumulated masks. The accumulated masks are then cleared which frees up the atlas
    // for future draws.
    //
    // TODO: We should not clear all accumulated masks but cache masks over more than one frame.
    //
    // TODO: We may need a method to generate raster-generated masks in separate threads prior to
    // upload.
    std::unique_ptr<RasterPathAtlas> fRasterPathAtlas;

    // Allocated and cached texture proxies shared by all PathAtlas instances. It is possible for
    // the same texture to be bound to multiple DispatchGroups and DrawPasses across flushes. The
    // owning Recorder must guarantee that any uploads or compute dispatches are scheduled to remain
    // coherent across flushes.
    // TODO: This requirement might change with a more sophisticated reuse scheme for texture
    // allocations. For now our model is simple: all PathAtlases target the same texture and only
    // one of them will render to the texture during a given command submission.
    std::unordered_map<uint64_t, sk_sp<TextureProxy>> fTexturePool;

    SkEnumBitMask<PathAtlasFlags> fPathAtlasFlags = PathAtlasFlags::kNone;
};

SK_MAKE_BITMASK_OPS(AtlasProvider::PathAtlasFlags)

}  // namespace skgpu::graphite

#endif  // skgpu_graphite_AtlasProvider_DEFINED
