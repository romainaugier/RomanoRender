#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_TRIANGULATE

#include "scene.h"


void Object::set_transform()
{
    set_translation(transformation_matrix, translate);
    set_rotation(transformation_matrix, rotate);
    set_scale(transformation_matrix, scale);

    Vertex* vertices = (Vertex*)rtcGetGeometryBufferData(geometry, RTC_BUFFER_TYPE_VERTEX, 0);

    for(int i = 0; i < vtx_count; i++)
    {
        vec3 temp(orig_positions[i].x, orig_positions[i].y, orig_positions[i].z);

        vec3 temp2 = transform(temp, transformation_matrix);

        vertices[i].x = temp2.x;
        vertices[i].y = temp2.y;
        vertices[i].z = temp2.z;
    }

    rtcUpdateGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0);
}


RTCGeometry load_geometry(tinyobj::shape_t& shape, tinyobj::attrib_t& attrib, RTCDevice& g_device, std::string& name, Vertex* orig, int& size)
{
    RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

    // here we create 2 geometry buffer, one to store the original position, and one to store the transformed position
    Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), attrib.vertices.size());

    Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), shape.mesh.indices.size() / 3);

    int id = 0;

    for (auto i = 0; i < attrib.vertices.size(); i += 3)
    {
        vertices[id].x = attrib.vertices[i];
        vertices[id].y = attrib.vertices[i + 1];
        vertices[id].z = attrib.vertices[i + 2];

        orig[id].x = attrib.vertices[i];
        orig[id].y = attrib.vertices[i + 1];
        orig[id].z = attrib.vertices[i + 2];

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

    name = shape.name;
    size = attrib.vertices.size();

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
            int vtx_number;
            Vertex* orig = (Vertex*)malloc(sizeof(Vertex) * attrib.vertices.size());
            RTCGeometry current_geometry = load_geometry(shapes[i], attrib, g_device, name, orig, vtx_number);

            unsigned int m_id = i;
            Material new_mat(m_id);
            new_mat.name = name;

#pragma omp critical
            {
                objects.push_back(Object(m_id, name, new_mat, current_geometry, orig, vtx_number));
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

    g_scene = rtcNewScene(g_device);
    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);


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