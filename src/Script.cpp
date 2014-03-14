/*********************************************************************************
Codebyte Engine

Copyright (c) 2014 Cameron Kline

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*********************************************************************************/

#include "Script.h"

/********************** Static code **********************/
duk_context* Script::ctx = NULL;
ulong Script::nextID = 0;
std::vector<Script*> Script::scriptObjects;

Script* Script::At(ulong id){
    for(uint i = 0; i < (uint)scriptObjects.size(); i++){
        if(scriptObjects.at(i)->id == id) return scriptObjects.at(i);
    }
    return NULL;
}
Script* Script::At(String name){
    for(uint i = 0; i < (uint)scriptObjects.size(); i++){
        if(scriptObjects.at(i)->name == name) return scriptObjects.at(i);
    }
    std::cout << "Script Object with name: '" << name << "' does not exist!" << std::endl;
    return NULL;
}
void Script::Setup(){
    ctx = duk_create_heap_default();
    if (!ctx) { std::cout << "Failed to create duk_context!" << std::endl; }
    duk_push_global_object(ctx);
}
void Script::LoadScript(String fileLocation){
    if(ctx != NULL){
        if(FileExists(fileLocation)){
            duk_eval_file(ctx, fileLocation.c_str());
        }else{
            std::cout << "Script file at location '" << fileLocation << "' does not exist!" << std::endl;
        }
    }else{
        std::cout << "Script:LoadScript ctx is NULL, make sure to call Script::Setup!" << std::endl;
    }
}
void Script::Update(){
    for(uint i = 0; i < (uint)scriptObjects.size(); i++){
        if(scriptObjects.at(i)->obj_index > -1){
            duk_get_prop_string(ctx, scriptObjects.at(i)->obj_index, "update");
            if(duk_get_type(ctx, -1) != DUK_TYPE_UNDEFINED) duk_call(ctx, 0);
        }
    }
}
void Script::Cleanup(){
    for(uint i = 0; i < (uint)scriptObjects.size(); i++)
        delete scriptObjects.at(i);
    scriptObjects.clear();
    nextID = 0;
    duk_destroy_heap(ctx);
}
/*********************************************************/


Script::Script(String name, String objectName){
    // make sure that object exist
    Dbug(10);
    if(ctx != NULL){
        Dbug(11);
        duk_get_prop_string(ctx, 0, objectName.c_str());
        Dbug(12);
        if(duk_get_type(ctx, -1) != DUK_TYPE_UNDEFINED){
            Dbug(13);
            // now create it and store its position on the stack for later use
            duk_new(ctx, 0);
            this->obj_index = duk_get_top_index(ctx);
        }else{
            this->obj_index = -1;
            std::cout << "Javascript object '" << objectName << "' does not exist!" << std::endl;
        }
    }else{
        std::cout << "ctx is NULL, make sure to call Script::Setup!" << std::endl;
    }

    this->name = name;
    this->id = nextID;
    nextID++;
    scriptObjects.push_back(this);
}

Script::~Script(){

}
