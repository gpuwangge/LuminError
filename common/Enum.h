#pragma once
enum CameraType { LOCK, FREE, SELECT }; //SELECT is not implemented yet

enum RenderModes {
    GRAPHICS = 0,
    COMPUTE,
    COMPUTE_SWAPCHAIN,
    COMPUTE_GRAPHICS,
    GRAPHICS_SHADOWMAP,
    RAYTRACING_SWAPCHAIN,
    GRAPHICS_MESHSHADER
};

enum ShaderTypes{VERT, FRAG, COMP, RAYT, MESH, TASK};


