#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_TRIANGULATE

#include "scene.h"

RTCGeometry load_geometry(tinyobj::shape_t& shape, tinyobj::attrib_t& attrib, RTCDevice& g_device)
{
    RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

    Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), attrib.vertices.size());

    Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), shape.mesh.indices.size() / 3);

    int id = 0;

    for (auto i = 0; i < attrib.vertices.size(); i += 3)
    {
        vertices[id].x = attrib.vertices[i];
        vertices[id].y = attrib.vertices[i + 1];
        vertices[id].z = attrib.vertices[i + 2];

        //std::cout << vertices[id].x << "/" << vertices[id].y << "/" << vertices[id].z << "\n";
        id++;
    }

    for (auto i = 0; i < shape.mesh.indices.size() / 3; i++)
    {
        triangles[i].v0 = shape.mesh.indices[i * 3].vertex_index;
        triangles[i].v1 = shape.mesh.indices[i * 3 + 1].vertex_index;
        triangles[i].v2 = shape.mesh.indices[i * 3 + 2].vertex_index;

        //std::cout << triangles[i].v0 << "/" << triangles[i].v1 << "/" << triangles[i].v2 << "\n";
    }


    return geo;
}


void load_object(RTCDevice& g_device, std::string path, std::vector<Object>& objects, Console& console)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> tiny_obj_mats;

    std::string warn;
    std::string err;

    std::string base_dir = get_base_dir(path);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &tiny_obj_mats, &warn, &err, path.c_str(), base_dir.c_str(), true);


    if (ret)
    {
        std::string name;

#pragma omp parallel for
        for (int i = 0; i < shapes.size(); i++)
        {
            RTCGeometry current_geometry = load_geometry(shapes[i], attrib, g_device);

            unsigned int m_id = i;
            Material new_mat(m_id);
            new_mat.name = name;

#pragma omp critical
            {
                objects.push_back(Object(m_id, name, new_mat, current_geometry));
            }
        }

        console.AddLog("[MESSAGE] : Loaded %s", path.c_str());
    }
    else
    {
        console.AddLog("[ERROR] : Could not load file : %s%s", warn.c_str(), err.c_str());
    }

}


void build_scene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects, std::vector<Material>& scene_materials)
{
    int i = 0;

    for (auto object : objects)
    {
        rtcCommitGeometry(object.geometry);
        unsigned int geoID = rtcAttachGeometry(g_scene, object.geometry);
        //rtcReleaseGeometry(geo);

        scene_materials.push_back(object.material);

        i++;
    }

    rtcCommitScene(g_scene);
}


void rebuild_scene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects, std::vector<Material>& scene_materials)
{
    rtcReleaseScene(g_scene);

    int i = 0;

    for (auto object : objects)
    {
        rtcCommitGeometry(object.geometry);
        unsigned int geoID = rtcAttachGeometry(g_scene, object.geometry);
        //rtcReleaseGeometry(geo);

        scene_materials.push_back(object.material);

        i++;
    }

    rtcCommitScene(g_scene);
}