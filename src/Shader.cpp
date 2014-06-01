/*
 * Shader.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: tbabb
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "glHeaders.h"
#include "Shader.h"

using namespace std;

/***********************************
 * Helper functions                *
 ***********************************/


Shader shaderFromFile(string name, string filepath){
     ifstream in_vtx((filepath + ".vertex").c_str());
     ifstream in_frg((filepath + ".frag").c_str());
     string vtx_src = string((std::istreambuf_iterator<char>(in_vtx)), std::istreambuf_iterator<char>());
     string frg_src = string((std::istreambuf_iterator<char>(in_frg)), std::istreambuf_iterator<char>());
     in_vtx.close();
     in_frg.close();
     Shader s = Shader(name, vtx_src, frg_src);
     return s;
 }

Shader shaderFromFile(string name, string vtx_srcf, string geo_srcf, string frg_srcf, GLint input_geom, GLint output_geom, int generated_vtxs){
    ifstream in_vtx(vtx_srcf.c_str());
    ifstream in_geo(geo_srcf.c_str());
    ifstream in_frg(frg_srcf.c_str());
    string vtx_src = string((std::istreambuf_iterator<char>(in_vtx)), std::istreambuf_iterator<char>());
    string geo_src = string((std::istreambuf_iterator<char>(in_geo)), std::istreambuf_iterator<char>());
    string frg_src = string((std::istreambuf_iterator<char>(in_frg)), std::istreambuf_iterator<char>());
    in_vtx.close();
    in_geo.close();
    in_frg.close();
    return Shader(name, vtx_src, frg_src, geo_src, input_geom, output_geom, generated_vtxs);
}

Shader shaderFromFile(string name, string filepath, GLint input_geom, GLint output_geom, int generated_vtxs){
    return shaderFromFile(name, filepath + ".vertex", filepath + ".geom", filepath + ".frag", input_geom, output_geom, generated_vtxs);
}

 
void printLog(int id, bool isProgram){
	int status = 0;
	if (isProgram) glGetProgramiv(id, GL_LINK_STATUS, &status);
	else           glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (!status){
		int infoLen = -1;
		int charsWritten = 0;
		char *log;
		if (isProgram) glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLen);
		else           glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0){
			log = new char[infoLen];
			if (isProgram) glGetProgramInfoLog(id, infoLen, &charsWritten, log);
			else           glGetShaderInfoLog(id, infoLen, &charsWritten, log);
			std::cout << log << "\n";
			delete[] log;
		} else {
			std::cout << "COMPILE PROBLEM (0 messages)\n";
		}
	}
}

/***********************************
 * ShaderObject helper classes     *
 ***********************************/

/////// Shader Info ///////

SharedShaderInfo::SharedShaderInfo(std::string name, ShaderObject vtx_shd, ShaderObject frg_shd, GeomShaderObject geo_shd):
        name(name),
        vtx_shd(vtx_shd),
        frg_shd(frg_shd),
        geo_shd(geo_shd),
        prog_id(0),
        linked(false),
        useGeomShader(true),
        error(false){
    //do nothing else
}

SharedShaderInfo::SharedShaderInfo(std::string name, ShaderObject vtx_shd, ShaderObject frg_shd):
        name(name),
        vtx_shd(vtx_shd),
        frg_shd(frg_shd),
        geo_shd(),
        prog_id(0),
        linked(false),
        useGeomShader(false),
        error(false){
    //do nothing else
}

/////// Shader Object ///////

ShaderObject::ShaderObject(std::string src, GLint type):
        shader_src(src),
        id(0),
        type(type),
        compiled(false),
        error(false){
    //do nothing else
}

GeomShaderObject::GeomShaderObject(std::string src, GLint input_type, GLint output_type, int geo_vtxs):
        ShaderObject(src, GL_GEOMETRY_SHADER_EXT),
        input_type(input_type),
        output_type(output_type),
        geo_vtxs(geo_vtxs){
    //do nothing else
}

//a dummy empty GeomShaderObject
GeomShaderObject::GeomShaderObject():
        ShaderObject("", GL_GEOMETRY_SHADER_EXT),
        input_type(GL_TRIANGLES),
        output_type(GL_TRIANGLES),
        geo_vtxs(3){
    //do nothing else
}

bool ShaderObject::compile(){
    if (compiled or error) return error;
    int len = shader_src.length();
    GLint ok = GL_TRUE;
    const char* cstr_src = shader_src.c_str();
    
    id = glCreateShader(type);
    glShaderSource(id, 1, &cstr_src, &len);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    printLog(id, false);
    
    if (ok == GL_TRUE){
        compiled = true;
        error = false;
    } else {
        compiled = false;
        error = true;
    }
    return compiled;
}

void ShaderObject::detach(GLuint program){
    if (id && program){
        glDetachShader(program, id);
    }
}

void ShaderObject::release(){
    if (id){
        glDeleteShader(id);
        id = 0;
        compiled = false;
        error = false;
    }
}

void ShaderObject::clearError(){
    if (id && error){
        glDeleteShader(id);
        error = false;
        compiled = false;
    }
}

/***********************************
 * Shader Structors                *
 ***********************************/

Shader::Shader(string name, string vtx_src, string frg_src, string geo_src, GLint input_type, GLint output_type, int n_generated_vtxs):
       shader_info(new SharedShaderInfo(name, 
                   ShaderObject(vtx_src, GL_VERTEX_SHADER), 
                   ShaderObject(frg_src, GL_FRAGMENT_SHADER),
                   GeomShaderObject(geo_src, input_type, output_type, n_generated_vtxs))),
       isnull(false){
    //do nothing else
}

Shader::Shader(string name, string vtx_src, string frg_src):
       shader_info(new SharedShaderInfo(name, 
                   ShaderObject(vtx_src, GL_VERTEX_SHADER), 
                   ShaderObject(frg_src, GL_FRAGMENT_SHADER))),
       isnull(false){
    //do nothing else
}

Shader::Shader():
        isnull(true){
    //do nothing else
}
        

Shader::~Shader() {
    //do nothing
}

/***********************************
 * Shader functions                *
 ***********************************/

bool Shader::compile(){
    if (isnull) return false;
    if (shader_info->error || shader_info->linked) return !shader_info->error;
	//compile vertex and fragment shaders
    GLint new_prog_id = glCreateProgram();
	bool  vtx_ok  = shader_info->vtx_shd.compile();
	bool  frg_ok  = vtx_ok && shader_info->frg_shd.compile();
	bool  geom_ok = true;
	GLint prog_ok = false;

	//compile geometry shader if applicable and no failure
	if (shader_info->useGeomShader && frg_ok){
	    GeomShaderObject &geo_shd = shader_info->geo_shd;
	    geom_ok = geo_shd.compile();
		if (geom_ok){
            glAttachShader(new_prog_id, geo_shd.id);
            glProgramParameteriEXT(new_prog_id, GL_GEOMETRY_INPUT_TYPE_EXT, geo_shd.input_type);
            glProgramParameteriEXT(new_prog_id, GL_GEOMETRY_OUTPUT_TYPE_EXT, geo_shd.output_type);
            glProgramParameteriEXT(new_prog_id, GL_GEOMETRY_VERTICES_OUT_EXT, geo_shd.geo_vtxs);
		}
	}

	//attach vertex, frag shaders if no failure yet
	if (frg_ok && geom_ok){
        glAttachShader(new_prog_id, shader_info->frg_shd.id);
        glAttachShader(new_prog_id, shader_info->vtx_shd.id);

        //link
        glLinkProgram(new_prog_id);
        glGetProgramiv(new_prog_id, GL_LINK_STATUS, &prog_ok);
        printLog(new_prog_id, true);
	}
	
	shader_info->prog_id = new_prog_id;
	shader_info->linked  = prog_ok;
	shader_info->error   = !prog_ok;
	
	return frg_ok && geom_ok && prog_ok;
}

//return true on success
bool Shader::use(){
    //compile if not compiled already
    
    bool compiled = this->compile();
    if (!compiled){
        return false; 
    }
    glUseProgram(shader_info->prog_id);
    return true;
}

//return true on success
bool Shader::disable(){
	int used = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &used); 
	if (!isnull && used && used == shader_info->prog_id){
		glUseProgram(0);
		return true;
	} else {
		return false;
	}
}

GLint Shader::getUniformID(string name){
    if (isnull) return -1;
    //TODO: clean me
    GLint r = glGetUniformLocation(shader_info->prog_id, name.c_str());
	return r;
}

GLint Shader::getAttribID(string name){
    if (isnull) return -1;
	return glGetAttribLocation(shader_info->prog_id, name.c_str());
}

std::string Shader::getName(){
    if (isnull) return "null shader";
    return this->shader_info->name;
}

GLint Shader::getProgramID(){
    if (isnull) return 0;
    return this->shader_info->prog_id;
}

void Shader::release(){
    if (isnull) return;
    GLuint progid = shader_info->prog_id;
    if (progid){
        shader_info->vtx_shd.detach(progid);
        shader_info->frg_shd.detach(progid);
        shader_info->geo_shd.detach(progid);
        glDeleteProgram(shader_info->prog_id);
        shader_info->prog_id = 0;
        shader_info->linked = false;
        shader_info->error = false;
    }
    shader_info->vtx_shd.release();
    shader_info->frg_shd.release();
    shader_info->geo_shd.release();
}

void Shader::clearError(){
    if (isnull) return;
    if (shader_info->error){
        glDeleteProgram(shader_info->prog_id);
        shader_info->error = false;
        shader_info->linked = false;
    }
    shader_info->vtx_shd.clearError();
    shader_info->frg_shd.clearError();
    shader_info->geo_shd.clearError();
}

bool Shader::isNullShader(){
    return isnull;
}
