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

#include "Texture.h"

/********************** Static code **********************/
ulong Texture::nextID = 0;
std::vector<Texture*> Texture::textures;
Texture* Texture::At(ulong id){
    for(uint i = 0; i < (uint)textures.size(); i++){
        if(textures.at(i)->id == id) return textures.at(i);
    }
    return NULL;
}
Texture* Texture::At(String name){
    for(uint i = 0; i < (uint)textures.size(); i++){
        if(textures.at(i)->name == name) return textures.at(i);
    }
    std::cout << "Texture with name: " << name << " does not exist!" << std::endl;
    return NULL;
}
Texture* Texture::WithLocation(String location){
    for(uint i = 0; i < (uint)textures.size(); i++){
        if(textures.at(i)->fileLocation == location) return textures.at(i);
    }
    return NULL;
}
void Texture::Cleanup(){
    for(uint i = 0; i < (uint)textures.size(); i++)
        delete textures.at(i);
    textures.clear();
    nextID = 0;
}
/*********************************************************/

Texture::Texture(String name){
    this->name = name;
    this->id = nextID;
    this->fileLocation = "";
    nextID++;
    textures.push_back(this);
}

Texture::~Texture(void){
	glDeleteTextures(1, &textureId);
}

bool Texture::Load(String location){
    this->fileLocation = location;

    // Load the image
    std::vector<unsigned char> image;
    uint width, height;
    uint error = lodepng::decode(image, width, height, location.c_str());

    // Flips the image vertically (OpenGL needs you to do that)
    std::vector<unsigned char> imageFlipped;
    for(uint y = height - 1; y > 0; y--){
        for(uint x = 0; x < (width * 4); x++) {
            imageFlipped.push_back(image.at((y * (width * 4)) + x));
        }
    }
    // Create and upload the image to the GPU
    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageFlipped[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture::Bind(Shader* shader, GLuint location, GLuint textureSlot){
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
    glUniform1i(location, textureSlot);
}
