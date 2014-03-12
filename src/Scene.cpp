#include "Scene.h"

Node* Scene::base = NULL;

void Scene::Setup(){
    base = new Node("base");
}

void Scene::Cleanup(){
    Node::Cleanup();
    Camera::Cleanup();
    Shader::Cleanup();
    Mesh::Cleanup();
    Texture::Cleanup();
    Material::Cleanup();
    Physics::Cleanup();
}

void Scene::LoadMap(String filename){
    // Make sure to cleanup before loading new stuff
    Cleanup();
    Physics::Setup(glm::vec3(0, -9.81f, 0), 1.0f / 60.0f);

    // Make sure to add the base node
    base = new Node("base");

    StringArray fileContents = LoadFile(filename);
    String line = "";
    int currentResourceGroup = -1;

    for(uint i = 0; i < (uint)fileContents.size(); i++){
        line = fileContents.at(i);
        line = RemoveCharToChar(line, '#', '\n');
        if(line == "[models]")          currentResourceGroup = 0;
        else if(line == "[textures]")   currentResourceGroup = 1;
        else if(line == "[shaders]")    currentResourceGroup = 2;
        else if(line == "[materials]")  currentResourceGroup = 3;
        else if(line == "[cameras]")    currentResourceGroup = 4;
        else if(line == "[physics]")    currentResourceGroup = 5;
        else if(line == "[nodes]")      currentResourceGroup = 6;
        else if(line.size() > 0 || line.substr(0, 1) != "#"){ // Its probably a resource
            switch(currentResourceGroup){
                case 0: {
                    // [format] name : filelocation
                    StringArray tokens = SplitString(line, ":");
                    Mesh* temp = new Mesh(tokens.at(0));
                    temp->Load(tokens.at(1));
                } break;
                case 1: {
                    // [format] name : filelocation
                    StringArray tokens = SplitString(line, ":");
                    Texture* temp = new Texture(tokens.at(0));
                    temp->Load(tokens.at(1));
                } break;
                case 2: {
                    // [format] name : filelocation
                    StringArray tokens = SplitString(line, ":");
                    Shader* temp = new Shader(tokens.at(0));
                    temp->Load(tokens.at(1));
                } break;
                case 3: {
                    // [format] name : filelocation
                    StringArray tokens = SplitString(line, ":");
                    Material* temp = new Material(tokens.at(0));
                    temp->Load(tokens.at(1));
                } break;
                case 4: {
                    // [format] name : fov : aspect
                    StringArray tokens = SplitString(line, ":");
                    if((uint)tokens.size() == 3)
                        Camera* temp = new Camera(tokens.at(0), std::atof(tokens.at(1).c_str()), 800.0f / 600.0f);
                    else
                        std::cout << "Invalid camera format in map file '" << filename << "' on line " << (i + 1) << std::endl;
                } break;
                case 5: {
                    // [format] name : position : rotation : scale : mass : shape
                    StringArray tokens = SplitString(line, ":");
                    if((uint)tokens.size() == 6){
                        StringArray pos = SplitString(tokens.at(1), " ");
                        StringArray rot = SplitString(tokens.at(2), " ");
                        StringArray scale = SplitString(tokens.at(3), " ");
                        Physics* temp = new Physics(tokens.at(0), Transform(
                                glm::vec3(std::atof(pos.at(0).c_str()), std::atof(pos.at(1).c_str()), std::atof(pos.at(2).c_str())),
                                glm::quat(glm::vec3(std::atof(rot.at(0).c_str()), std::atof(rot.at(1).c_str()), std::atof(rot.at(2).c_str()))),
                                glm::vec3(std::atof(scale.at(0).c_str()), std::atof(scale.at(1).c_str()), std::atof(scale.at(2).c_str()))
                            ), std::atoi(tokens.at(4).c_str()), tokens.at(5)
                        );
                    }else{
                        std::cout << "Invalid physics object format in map file '" << filename << "' on line " << (i + 1) << std::endl;
                    }
                } break;
                case 6: {
                    // [format] name : parent : position : rotation : scale : meshId : materialId : cameraId : physicsId
                    StringArray tokens = SplitString(line, ":");
                    if(tokens.size() == 9){
                        StringArray pos = SplitString(tokens.at(2), " ");
                        StringArray rot = SplitString(tokens.at(3), " ");
                        StringArray scale = SplitString(tokens.at(4), " ");

                        CreateNode(tokens.at(0), Node::At(tokens.at(1)),
                            Transform(
                                glm::vec3(std::atof(pos.at(0).c_str()), std::atof(pos.at(1).c_str()), std::atof(pos.at(2).c_str())),
                                glm::quat(glm::vec3(std::atof(rot.at(0).c_str()), std::atof(rot.at(1).c_str()), std::atof(rot.at(2).c_str()))),
                                glm::vec3(std::atof(scale.at(0).c_str()), std::atof(scale.at(1).c_str()), std::atof(scale.at(2).c_str()))
                            ),
                            ((tokens.at(5) == "none") ? NULL : Mesh::At(tokens.at(5))),
                            ((tokens.at(6) == "none") ? NULL : Material::At(tokens.at(6))),
                            ((tokens.at(7) == "none") ? NULL : Camera::At(tokens.at(7))),
                            ((tokens.at(8) == "none") ? NULL : Physics::At(tokens.at(8)))
                        );
                    }else{
                        std::cout << "Invalid node format in map file '" << filename << "' on line " << (i + 1) << std::endl;
                    }
                } break;
            }
        }
    }

}

void Scene::SaveMap(String filename){

}

// We use pointers here in case we dont want one of them (therefore it'd be NULL)
void Scene::CreateNode(String name, Node* parent, Transform transform, Mesh* mesh, Material* material, Camera* camera, Physics* physics){
    Node* temp = new Node(name, transform);
    parent->AddChild(temp);
    if(mesh != NULL)        temp->SetMesh(mesh);
    if(material != NULL)    temp->SetMaterial(material);
    if(camera != NULL)      temp->SetCamera(camera);
    if(physics != NULL)     temp->SetPhysics(physics);
    // Sort the list by shader (might speed up rendering)
    Node::Sort();
}

void Scene::RemoveNode(ulong nodeId){

}

void Scene::Update(){
    glm::mat4 * mat = new glm::mat4(1.0f);
    base->Update(mat);
    delete mat;
}

void Scene::Render(){
    // This function will do some occlusion culling magic
    // Then only render things that need rendered
    //base->Render(Camera::ProjViewMat());
    GLuint lastShaderId = 0;
    for(uint i = 0; i < (uint)Node::nodes.size(); i++){
        if(Node::nodes.at(i)->meshPtr != NULL && Node::nodes.at(i)->materialPtr != NULL){
            // Make sure where using the right shader
            if(Node::nodes.at(i)->materialPtr->shader->shaderID != lastShaderId){
                Node::nodes.at(i)->materialPtr->shader->Bind();
                lastShaderId = Node::nodes.at(i)->materialPtr->shader->shaderID;
            }

            // Upload material attributes
            Node::nodes.at(i)->materialPtr->Upload(Node::nodes.at(i)->materialPtr->shader);

            Node::nodes.at(i)->Render(Camera::ProjViewMat());
        }
    }
}
