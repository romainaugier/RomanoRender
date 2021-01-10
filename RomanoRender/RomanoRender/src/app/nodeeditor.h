#pragma once

#include "imnodes.h"
#include "imgui.h"
#include "nodes.h"
#include "console.h"
#include "scene/settings.h"


struct Node_Editor
{
    imnodes::EditorContext* context;
    std::vector<Node> nodes;
    std::vector<Link> links;
    int current_id = 1;
    int current_link_id = 0;
    int node_ids[8];

    Node_Editor() 
    {
        context = imnodes::EditorContextCreate();
        imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);

        imnodes::IO& io = imnodes::GetIO();
        bool is_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        io.link_detach_with_modifier_click.modifier = &ImGui::GetIO().MouseClicked[0];

        for (int i = 0; i < sizeof(node_ids) / sizeof(int); i++)
        {
            node_ids[i] = 0;
        }
    }


    ~Node_Editor()
    {
        imnodes::PopAttributeFlag();
        imnodes::EditorContextFree(context);
    }


    void Draw(const char* editor_name, Console& console, Node& selected_node, Render_Settings& settings, std::vector<material>& materials, std::vector<camera>& cameras, std::vector<Light>& lights, int& change, bool& edited)
    {
        imnodes::EditorContextSet(context);

        ImGui::Begin(editor_name);

        ImGui::TextUnformatted("TAB : Add node");

        imnodes::BeginNodeEditor();


        const bool open_popup = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) && imnodes::IsEditorHovered();

        if (!ImGui::IsAnyItemHovered() && open_popup)
        {
            ImGui::OpenPopup("Add Node");
        }


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));


        if (ImGui::BeginPopup("Add Node"))
        {
            const ImVec2 node_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

            if (ImGui::MenuItem("Geometry"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[0]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Geometry_%d", node_ids[0]);

                const Node geo(Node_Type::Geometry, node_id, node_name);

                nodes.push_back(geo);
                console.AddLog("Geometry Node Created");

                node_ids[0]++;
                current_id++;
            }

            if (ImGui::MenuItem("Surface"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[1]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Surface_%d", node_ids[1]);

                const Node geo(Node_Type::Surface, node_id, node_name);

                nodes.push_back(geo);
                console.AddLog("Surface Node Created");

                node_ids[1]++;
                current_id++;
            }

            if (ImGui::MenuItem("Light"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[2]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Light_%d", node_ids[2]);

                Node node_light(Node_Type::Light, node_id, node_name);

                node_light.light.light_type = Light_Type::Point;
                node_light.light.has_map = false;

                nodes.push_back(node_light);
                lights.push_back(node_light.light);

                console.AddLog("Light Node Created");
                
                node_ids[2]++;
                current_id++;
            }

            if (ImGui::MenuItem("Camera"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[3]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Camera_%d", node_ids[3]);

                Node node_cam(Node_Type::Camera, node_id, node_name);

                node_cam.cam.pos = Vec3(0.0f, 7.5f, 30.0f);
                node_cam.cam.lookat = Vec3(0.0f, 7.5f, 0.0f);
                node_cam.cam.focal_length = 50.0f;
                node_cam.cam.focus_dist = 20.0f;
                node_cam.cam.update(settings.xres, settings.yres);
                node_cam.cam.aperture = 0.0f;

                nodes.push_back(node_cam);
                cameras.push_back(node_cam.cam);

                std::cout << cameras.size() << "\n";

                console.AddLog("Camera Node Created");

                node_ids[3]++;
                current_id++;
            }

            if (ImGui::MenuItem("Settings"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[4]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Settings_%d", node_ids[4]);

                const Node geo(Node_Type::Settings, node_id, node_name);

                nodes.push_back(geo);
                console.AddLog("Settings Node Created");
                
                node_ids[4]++;
                current_id++;
            }

            if (ImGui::MenuItem("Merge"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[5]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Merge_%d", node_ids[5]);

                const Node geo(Node_Type::Merge, node_id, node_name);

                nodes.push_back(geo);
                console.AddLog("Merge Node Created");

                node_ids[5]++;
                current_id++;
            }

            if (ImGui::MenuItem("Attribute"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[6]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Output_%d", node_ids[6]);

                const Node attr(Node_Type::Attributes, node_id, node_name);

                nodes.push_back(attr);
                console.AddLog("Attribute Node Created");

                current_id++;
            }

            if (ImGui::MenuItem("Output"))
            {
                const int node_id = current_id;

                imnodes::SetNodeScreenSpacePos(node_id, node_pos);

                static char node_name[(((sizeof node_ids[6]) * CHAR_BIT) + 2) / 3 + 2];
                sprintf(node_name, "Output_%d", node_ids[6]);

                const Node geo(Node_Type::Output, node_id, node_name);

                nodes.push_back(geo);
                console.AddLog("Output Node Created");

                node_ids[6]++;
                current_id++;
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
        

        for (Node& node : nodes)
        {
            float node_witdh = 150.0f;

            switch (node.type)
            {

            case Node_Type::Geometry:
            {
                imnodes::BeginNode(node.id);

                {
                    imnodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name);
                    imnodes::EndNodeTitleBar();

                    imnodes::BeginOutputAttribute(node.id << 24);

                    const float txt_size = ImGui::CalcTextSize("Output").x;
                    ImGui::Indent(node_witdh - txt_size);
                    ImGui::TextUnformatted("Output");
                    imnodes::EndOutputAttribute();
                }

                imnodes::EndNode();
            }
            break;

            case Node_Type::Surface:
            {
                imnodes::BeginNode(node.id);

                {
                    imnodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name);
                    imnodes::EndNodeTitleBar();

                    imnodes::BeginInputAttribute(node.id << 8);
                    ImGui::TextUnformatted("Input");
                    imnodes::EndInputAttribute();

                    imnodes::BeginOutputAttribute(node.id << 24);
                    const float txt_size = ImGui::CalcTextSize("Output").x;
                    ImGui::Indent(node_witdh - txt_size);
                    ImGui::TextUnformatted("Output");
                    imnodes::EndOutputAttribute();

                    if (node.from.size() > 0 && change > 0)
                    {
                        node.geometry = node.from[0].geometry;
                        node.materials = node.from[0].materials;
                    }
                }

                imnodes::EndNode();
            }
            break;

            case Node_Type::Light:
            {
                imnodes::BeginNode(node.id);

                {
                    imnodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name);
                    imnodes::EndNodeTitleBar();

                    imnodes::BeginOutputAttribute(node.id << 24);
                    const float txt_size = ImGui::CalcTextSize("Output").x;
                    ImGui::Indent(node_witdh - txt_size);
                    ImGui::TextUnformatted("Output");
                    imnodes::EndOutputAttribute();
                }

                imnodes::EndNode();
            }
            break;

            case Node_Type::Camera:
                {
                    imnodes::BeginNode(node.id);

                    {
                        imnodes::BeginNodeTitleBar();
                        ImGui::TextUnformatted(node.name);
                        imnodes::EndNodeTitleBar();
                        
                        imnodes::BeginOutputAttribute(node.id << 24);
                        const float txt_size = ImGui::CalcTextSize("Output").x;
                        ImGui::Indent(node_witdh - txt_size);
                        ImGui::TextUnformatted("Output");
                        imnodes::EndOutputAttribute();
                    }

                    imnodes::EndNode();
                }
            break;
            
            case Node_Type::Settings:
                {
                    imnodes::BeginNode(node.id);

                    {
                        imnodes::BeginNodeTitleBar();
                        ImGui::TextUnformatted(node.name);
                        imnodes::EndNodeTitleBar();

                        imnodes::BeginOutputAttribute(node.id << 24);
                        const float txt_size = ImGui::CalcTextSize("Output").x;
                        ImGui::Indent(node_witdh - txt_size);
                        ImGui::TextUnformatted("Output");
                        imnodes::EndOutputAttribute();
                    }

                    imnodes::EndNode();
                }
            break;

            case Node_Type::Output:
                {
                    imnodes::BeginNode(node.id);

                    {
                        imnodes::BeginNodeTitleBar();
                        ImGui::TextUnformatted(node.name);
                        imnodes::EndNodeTitleBar();

                        imnodes::BeginInputAttribute(node.id << 8);
                        ImGui::TextUnformatted("Input");
                        node_witdh = 100.0f;
                        ImGui::Indent(node_witdh);
                        imnodes::EndInputAttribute();

                        if (node.from.size() > 0)
                        {
                            // update input geometry
                            if (change > 0)
                            {
                                rtcReleaseScene(settings.scene);

                                settings.scene = rtcNewScene(settings.device);
                                rtcSetSceneBuildQuality(settings.scene, RTC_BUILD_QUALITY_HIGH);
                                rtcSetSceneFlags(settings.scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

                                materials.clear();

                                for (Node& in_node : node.from)
                                {
                                    if (in_node.type == Node_Type::Geometry || in_node.type == Node_Type::Surface || in_node.type == Node_Type::Merge)
                                    {
                                        SendToScene(settings.device, settings.scene, in_node.geometry, materials, in_node.materials);

                                        //console.AddLog("Geometry added to scene");
                                    }

                                    if (in_node.type == Node_Type::Light)
                                    {
                                        lights.push_back(in_node.light);

                                        // add light geometry to scene if the light is visible
                                        // kinda hardcoded right now

                                        if (in_node.light.light_type == Light_Type::Square && in_node.light.visible == true)
                                        {
                                            RTCGeometry geo = rtcNewGeometry(settings.device, RTC_GEOMETRY_TYPE_TRIANGLE);
                                            Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 4);

                                            vertices[0].x = in_node.light.v0.x;
                                            vertices[0].y = in_node.light.v0.y;
                                            vertices[0].z = in_node.light.v0.z;

                                            vertices[1].x = in_node.light.v1.x;
                                            vertices[1].y = in_node.light.v1.y;
                                            vertices[1].z = in_node.light.v1.z;

                                            vertices[2].x = in_node.light.v2.x;
                                            vertices[2].y = in_node.light.v2.y;
                                            vertices[2].z = in_node.light.v2.z;

                                            vertices[3].x = in_node.light.v3.x;
                                            vertices[3].y = in_node.light.v3.y;
                                            vertices[3].z = in_node.light.v3.z;

                                            Vertex* normals = new Vertex[4];

                                            for (int i = 0; i < 4; i++)
                                            {
                                                normals[i].x = in_node.light.orientation.x;
                                                normals[i].y = in_node.light.orientation.y;
                                                normals[i].z = in_node.light.orientation.z;
                                            }

                                            rtcSetGeometryVertexAttributeCount(geo, 1);
                                            rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(Vertex), 4);

                                            Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);

                                            triangles[0].v0 = 0;
                                            triangles[0].v1 = 1;
                                            triangles[0].v2 = 2;

                                            triangles[1].v0 = 1;
                                            triangles[1].v1 = 3;
                                            triangles[1].v2 = 2;

                                            rtcCommitGeometry(geo);
                                            unsigned int geoID = rtcAttachGeometry(settings.scene, geo);

                                            material new_material(geoID, in_node.light.color, in_node.light.orientation, true);
                                            materials.push_back(new_material);
                                        }
                                    }
                                }

                                rtcCommitScene(settings.scene);
                            }


                            // update input lights (this won't trigger geometry reloading)
                            if (change > 0 || edited)
                            {
                                lights.clear();

                                for (Node& in_node : node.from)
                                {
                                    if (in_node.type == Node_Type::Light)
                                    {
                                        lights.push_back(in_node.light);
                                    }
                                }
                            }
                        }
                    }

                    imnodes::EndNode();
                }
            break;

            case Node_Type::Merge:
            {
                imnodes::BeginNode(node.id);

                {
                    imnodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name);
                    imnodes::EndNodeTitleBar();

                    imnodes::BeginInputAttribute(node.id << 8);
                    ImGui::TextUnformatted("Input");
                    imnodes::EndInputAttribute();

                    imnodes::BeginOutputAttribute(node.id << 24);
                    const float txt_size = ImGui::CalcTextSize("Output").x;
                    node_witdh = 100.0f;
                    ImGui::Indent(node_witdh - txt_size);
                    ImGui::TextUnformatted("Output");
                    imnodes::EndOutputAttribute();

                    if (node.from.size() > 0)
                    {
                        node.geometry = node.from[0].geometry;
                        node.materials = node.from[0].materials;
                    }
                }

                imnodes::EndNode();
            }
            break;

            case Node_Type::Attributes:
            {
                imnodes::BeginNode(node.id);

                {
                    imnodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name);
                    imnodes::EndNodeTitleBar();

                    imnodes::BeginInputAttribute(node.id << 8);
                    ImGui::TextUnformatted("Input");
                    imnodes::EndInputAttribute();

                    imnodes::BeginOutputAttribute(node.id << 24);
                    const float txt_size = ImGui::CalcTextSize("Output").x;
                    node_witdh = 100.0f;
                    ImGui::Indent(node_witdh - txt_size);
                    ImGui::TextUnformatted("Output");
                    imnodes::EndOutputAttribute();

                    if (node.from.size() > 0)
                    {
                        node.geometry = node.from[0].geometry;
                        node.materials = node.from[0].materials;
                    }
                }

                imnodes::EndNode();
            }

            }
        }


        for (const Link& l : links)
        {
            imnodes::Link(l.id, l.start_attr, l.end_attr);
        }

        imnodes::EndNodeEditor();
        
        
        {
            Link l;
            if (imnodes::IsLinkCreated(&l.start_attr, &l.end_attr))
            {
                l.id = ++current_link_id;
                links.push_back(l);
                edited = true;
            }
        }


        {
            int link_id;
            if (imnodes::IsLinkDestroyed(&link_id))
            {
                auto iter = std::find_if(links.begin(), links.end(), [link_id](const Link& l) -> bool
                    {
                        return l.id == link_id;
                    });

                assert(iter != links.end());
                links.erase(iter);
                edited = true;
            }
        }

        
        for (Node& node : nodes)
        {
            node.from.clear();
            node.to.clear();
        }


        {
            int temp_node_id_to = 0;
            int temp_node_id_from = 0;

            for (Link& link : links)
            {
                int from = link.start_attr >> 24;
                int to = link.end_attr >> 8;

                for (Node& current_node : nodes)
                {
                    if (from == current_node.id)
                    {
                        temp_node_id_to = 0;

                        for (auto s_node : nodes)
                        {
                            if (s_node.id == to)
                            {
                                break;
                            }
                            temp_node_id_to++;
                        }
                        
                        current_node.to.push_back(nodes[temp_node_id_to]);
                    }

                    if (to == current_node.id)
                    {
                        temp_node_id_from = 0;

                        for (auto s_node : nodes)
                        {
                            if (s_node.id == from)
                            {
                                break;
                            }
                            temp_node_id_from++;
                        }

                        current_node.from.push_back(nodes[temp_node_id_from]);
                    }
                }
                
            }
        }
        

        {
            const int selected = imnodes::NumSelectedNodes();
            int temp_node_id = 0;

            if (selected > 0)
            {
                static std::vector<int> selected_nodes;
                selected_nodes.resize(static_cast<size_t>(selected));
                imnodes::GetSelectedNodes(selected_nodes.data());

                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
                {
                    for (const int node_id : selected_nodes)
                    {
                        auto iter = std::find_if(nodes.begin(), nodes.end(), [node_id](const Node& node) -> bool { return node.id == node_id; });
                        nodes.erase(iter);
                    }
                }

                else if (nodes.size() > 0)
                {
                    for (auto s_node : nodes)
                    {
                        if (s_node.id == selected_nodes[0]) break;
                        temp_node_id++;
                    }

                    if (temp_node_id < nodes.size())
                    {
                        selected_node = nodes[temp_node_id];
                    }
                }
            }
        }

        
        ImGui::End();
    }


};


struct Parameter_Editor
{

    Parameter_Editor() {}

    ~Parameter_Editor() {}

    void Draw(int& change, bool& edited, Node& selected_node, std::vector<Node>& nodes, std::vector<material>& materials, Render_Settings& settings, std::vector<camera>& cameras, std::vector<Light>& lights, Console& console)
    {
        ImGui::Begin("Parameters");

        if (nodes.size() > 0)
        {
            if (selected_node.type == Node_Type::None)
            {
                ImGui::BeginChild("EditorNone");
                ImGui::TextUnformatted(selected_node.name);
                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Geometry)
            {
                ImGui::BeginChild("EditorGeo");
                ImGui::TextUnformatted(selected_node.name);

                static char path[512] = "";
                //memcpy(path, selected_node.path, 512);

                ImGui::InputText("Path", path, IM_ARRAYSIZE(path));

                ImGui::SameLine();

                int geom_id = 0;

                for (auto node : nodes)
                {
                    if (node.id == selected_node.id) break;
                    else geom_id++;
                }

                if (ImGui::Button("Load"))
                {
                    if (FileExists(std::string(path)))
                    {
                        nodes[geom_id].materials.clear();
                        nodes[geom_id].geometry.clear();

                        ImGui::OpenPopup("Loading");

                        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2));

                        if (ImGui::BeginPopup("Loading"))
                        {
                            nodes[geom_id].geometry = LoadObject(settings.device, path, nodes[geom_id].materials, console);
                            change = 2;

                            ImGui::TextUnformatted("Loading Object...");
                            ImGui::EndPopup();
                        }
                    }
                    else
                    {
                        console.AddLog("[ERROR] : File does not exist.");
                    }
                }

                //memcpy(selected_node.path, path, sizeof(path));

                //console.AddLog(std::to_string(nodes[geom_id].geometry.size()).c_str());

                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Surface)
            {
                ImGui::BeginChild("EditorSurf");
                ImGui::TextUnformatted(selected_node.name);

                int surf_id = 0;

                for (auto node : nodes)
                {
                    if (node.id == selected_node.id) break;
                    else surf_id++;
                }

                static int mat_id = 0;

                if (nodes[surf_id].materials.size() > 0)
                {
                    if (ImGui::InputInt("Material ID", &mat_id))
                    {
                        mat_id = std::min(mat_id, (int)nodes[surf_id].materials.size() - 1);
                        mat_id = std::max(0, mat_id);
                        //mat_name = materials[mat_id].name.c_str();
                    }

                    {
                        ImGui::Text("%s", nodes[surf_id].materials[mat_id].name.c_str());

                        ImGui::ColorEdit3("Diffuse Color", &nodes[surf_id].materials[mat_id].clr.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Diffuse Roughness", &nodes[surf_id].materials[mat_id].diffuse_roughness, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Reflectance", &nodes[surf_id].materials[mat_id].reflectance, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::ColorEdit3("Reflectance Color", &nodes[surf_id].materials[mat_id].reflection_color.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Metallic", &nodes[surf_id].materials[mat_id].metallic, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Specular", &nodes[surf_id].materials[mat_id].specular, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Reflectance/Specular Roughness", &nodes[surf_id].materials[mat_id].roughness, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::ColorEdit3("Specular Color", &nodes[surf_id].materials[mat_id].specular_color.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Ior", &nodes[surf_id].materials[mat_id].ior.x);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Refraction", &nodes[surf_id].materials[mat_id].refraction, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("Refraction Roughness", &nodes[surf_id].materials[mat_id].refraction_roughness, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::ColorEdit3("Refraction Color", &nodes[surf_id].materials[mat_id].refraction_color.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("SSS Amount", &nodes[surf_id].materials[mat_id].sss, 0.05f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::ColorEdit3("SSS Color", &nodes[surf_id].materials[mat_id].sss_color.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("SSS Scale", &nodes[surf_id].materials[mat_id].sss_scale, 0.05f, 0.0f, 50.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::ColorEdit3("SSS Radius", &nodes[surf_id].materials[mat_id].sss_radius.x, ImGuiColorEditFlags_Float);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragFloat("SSS Absorption", &nodes[surf_id].materials[mat_id].sss_abs, 0.01f, 0.0f, 1.0f);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        ImGui::DragInt("SSS Max Depth", &nodes[surf_id].materials[mat_id].sss_steps);
                        if (ImGui::IsItemEdited())
                        {
                            edited = true;
                        }

                        nodes[surf_id].from[0].materials = nodes[surf_id].materials;


                        for (auto& mat : materials)
                        {
                            if (nodes[surf_id].materials[mat_id].mat_id == mat.mat_id)
                            {
                                mat = nodes[surf_id].materials[mat_id];
                            }
                        }
                        

                    }
                }

                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Light)
            {
                ImGui::BeginChild("EditorLight");
                ImGui::TextUnformatted(selected_node.name);

                int light_id = 0;

                for (auto node : nodes)
                {
                    if (node.id == selected_node.id) break;
                    else light_id++;
                }

                static int current_item = 0;

                if (nodes[light_id].light.light_type == Light_Type::Point) current_item = 0;
                if (nodes[light_id].light.light_type == Light_Type::Distant) current_item = 1;
                if (nodes[light_id].light.light_type == Light_Type::Square) current_item = 2;
                if (nodes[light_id].light.light_type == Light_Type::Dome) current_item = 3;

                int previous_item = current_item;
                const char* items[] = { "Point", "Distant", "Square", "Dome" };

                ImGui::Combo("Light Type", &current_item, items, IM_ARRAYSIZE(items));

                if (current_item != previous_item)
                {
                    edited = true;

                    if (current_item == 0)
                    {
                        nodes[light_id].light.light_type = Light_Type::Point;
                    }

                    if (current_item == 1)
                    {
                        nodes[light_id].light.light_type = Light_Type::Distant;
                    }

                    if (current_item == 2)
                    {
                        nodes[light_id].light.light_type = Light_Type::Square;
                    }

                    if (current_item == 3)
                    {
                        nodes[light_id].light.light_type = Light_Type::Dome;
                    }

                }

                ImGui::DragFloat("Intensity", &nodes[light_id].light.intensity, 0.01f);
                if (ImGui::IsItemEdited())
                {
                    edited = true;
                }

                ImGui::ColorEdit3("Color", &nodes[light_id].light.color.x, ImGuiColorEditFlags_Float);
                if (ImGui::IsItemEdited())
                {
                    edited = true;
                }

                if (nodes[light_id].light.light_type == Light_Type::Point)
                {
                    ImGui::DragFloat3("Position", &nodes[light_id].light.position.x, 0.1f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                }

                if (nodes[light_id].light.light_type == Light_Type::Distant)
                {
                    ImGui::DragFloat3("Direction", &nodes[light_id].light.direction.x, 0.01f, -1.0f, 1.0f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                    ImGui::DragFloat("Angle", &nodes[light_id].light.angle, 0.01f, 0.0f, 10000.0f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                }

                if (nodes[light_id].light.light_type == Light_Type::Square)
                {
                    ImGui::DragFloat3("Position", &nodes[light_id].light.position.x, 0.01f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                        nodes[light_id].light.InitializeSquare();
                    }

                    ImGui::DragFloat3("Orientation", &nodes[light_id].light.orientation.x, 0.01f, -1.0f, 1.0f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                        nodes[light_id].light.InitializeSquare();
                    }

                    ImGui::DragFloat2("Size", &nodes[light_id].light.area_size.x, 0.01f, 0.0f, 100000000.0f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                        nodes[light_id].light.InitializeSquare();
                    }


                }

                if (nodes[light_id].light.light_type == Light_Type::Dome)
                {
                    static char path[512] = "Path to your hdri";

                    ImGui::InputText("Path", path, IM_ARRAYSIZE(path));

                    ImGui::SameLine();

                    if (ImGui::Button("Load"))
                    {
                        if (FileExists(path))
                        {
                            nodes[light_id].light.InitializeHDRI(path);
                            console.AddLog("[MESSAGE] : HDRI ", path, " loaded.");

                            edited = true;
                        }
                        else
                        {
                            console.AddLog("[ERROR] : File does not exist.");
                        }
                    }

                    if (ImGui::Checkbox("Visible", &nodes[light_id].light.visible))
                    {
                        edited = true;
                    }
                }

                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Camera)
            {
                ImGui::BeginChild("EditorCam");
                ImGui::TextUnformatted(selected_node.name);

                int cam_id = 0;

                for (auto node : nodes)
                {
                    if (node.id == selected_node.id) break;
                    else cam_id++;
                }

                {
                    ImGui::DragFloat3("Camera Position", &nodes[cam_id].cam.pos.x, 0.05f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                    ImGui::DragFloat3("Camera Aim", &nodes[cam_id].cam.lookat.x, 0.05f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                    ImGui::DragFloat("Focal Length", &nodes[cam_id].cam.focal_length, 0.1f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                    ImGui::DragFloat("Bokeh Power", &nodes[cam_id].cam.aperture, 0.01f, 0.0f, 10.0f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                    ImGui::DragFloat("Focus Distance", &nodes[cam_id].cam.focus_dist, 0.01f);
                    if (ImGui::IsItemEdited())
                    {
                        edited = true;
                    }

                }

                nodes[cam_id].cam.update(settings.xres, settings.yres);

                cameras[0] = nodes[cam_id].cam;


                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Attributes)
            {
                ImGui::BeginChild("EditorAttribs");
                ImGui::TextUnformatted(selected_node.name);

                int attr_id = 0;

                for (auto node : nodes)
                {
                    if (node.id == selected_node.id) break;
                    else attr_id++;
                }

                static float transform[16] = { 1, 0, 0, 0,
                                              0, 1, 0, 0,
                                              0, 0, 1, 0,
                                              0, 0, 0, 1 };

                ImGui::DragFloat3("Translate", &transform[13], 0.05f);
                if (ImGui::IsItemEdited())
                {
                    change = 2;
                }
                //ImGui::DragFloat3("Rotate", &)

                for (auto geo : nodes[attr_id].geometry)
                {
                    rtcSetGeometryTransform(geo, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR, transform);
                }

                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Settings)
            {
                ImGui::BeginChild("EditorSet");
                ImGui::TextUnformatted(selected_node.name);

                ImGui::EndChild();
            }

            if (selected_node.type == Node_Type::Output)
            {
                ImGui::BeginChild("EditorOut");
                ImGui::TextUnformatted(selected_node.name);

                if (ImGui::Button("Reload Scene")) change = 1;

                ImGui::EndChild();
            }
        

        }

        ImGui::End();
    }
};