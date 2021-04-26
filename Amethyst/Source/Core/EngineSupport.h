#pragma once
#include <vector>
#include <string>

namespace Amethyst
{
    static const char* Extension_World       = ".world";
    static const char* Extension_Material    = ".material";
    static const char* Extension_Model       = ".model";
    static const char* Extension_Prefab      = ".prefab";
    static const char* Extension_Shader      = ".shader";
    static const char* Extension_Font        = ".font";
    static const char* Extension_Texture     = ".texture";
    static const char* Extension_Mesh        = ".mesh";
    static const char* Extension_Audio       = ".audio";
    static const char* Extension_Script      = ".cs";

    static const std::vector<std::string> SupportedImageFormats
    {
        ".jpg",
        ".png",
        ".bmp",
        ".tga",
        ".dds",
        ".exr",
        ".raw",
        ".gif",
        ".hdr",
        ".ico",
        ".iff",
        ".jng",
        ".jpeg",
        ".koala",
        ".kodak",
        ".mng",
        ".pcx",
        ".pbm",
        ".pgm",
        ".ppm",
        ".pfm",
        ".pict",
        ".psd",
        ".raw",
        ".sgi",
        ".targa",
        ".tiff",
        ".tif",     //tiff can also be known tif.
        ".wbmp",
        ".webp",
        ".xbm",
        ".xpm"
    };

    static const std::vector<std::string> SupportedAudioFormats
    {
        ".aiff",
        ".asf",
        ".asx",
        ".dls",
        ".flac",
        ".fsb",
        ".it",
        ".m3u",
        ".midi",
        ".mod",
        ".mp2",
        ".mp3",
        ".ogg",
        ".pls",
        ".s3m",
        ".vag",     //PS2/PSP
        ".wav",
        ".wax",
        ".wma",
        ".xm",
        ".xma"      //XBox 360
    };

    static const std::vector<std::string> SupportedModelFormats
    {
        ".3ds",
        ".obj",
        ".fbx",
        ".blend",
        ".dae",
        ".gltf",
        ".lwo",
        ".c4d",
        ".ase",
        ".dxf",
        ".hmp",
        ".md2",
        ".md3",
        ".md5",
        ".mdc",
        ".mdl",
        ".nff",
        ".ply",
        ".stl",
        ".x",
        ".smd",
        ".lxo",
        ".lws",
        ".ter",
        ".ac3d",
        ".ms3d",
        ".cob",
        ".q3bsp",
        ".xgl",
        ".csm",
        ".bvh",
        ".b3d",
        ".ndo"
    };

    static const std::vector<std::string> SupportedShaderFormats
    {
        ".hlsl"
    };

    static const std::vector<std::string> SupportedScriptFormats
    {
        ".cs",
        ".h",
        ".cpp"
    };

    static const std::vector<std::string> SupportedFontFormats
    {
        ".ttf",
        ".ttc",
        ".cff",
        ".woff",
        ".otf",
        ".otc",
        ".pfa",
        ".pfb",
        ".fnt",
        ".bdf",
        ".pfr"
    };
}

