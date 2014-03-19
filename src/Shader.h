/*
 * Shader.h
 * 
 * This class is SAFE to copy by value.
 *
 *  Created on: Jun 27, 2010
 *      Author: tbabb
 * 
 * TODO: destruction. release() called from SharedInfo destructor?
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <boost/shared_ptr.hpp>

/*****************************
 * Helper classes            *
 *****************************/

class ShaderObject {
public:
    ShaderObject(std::string src, GLint type);
    
    std::string shader_src;
    GLint id;
    GLint type;
    bool compiled;
    bool error;
    
    bool compile();
    void detach(GLuint program);
    void release();
    void clearError();
};

class GeomShaderObject : public ShaderObject{
public:
    GeomShaderObject(std::string src, GLint input_type, GLint output_type, int geo_vtxs);
    GeomShaderObject();
    
    GLint input_type;
    GLint output_type;
    int   geo_vtxs;
};

class SharedShaderInfo {
public:
    SharedShaderInfo(std::string name, ShaderObject vtx_shd, ShaderObject frg_shad, GeomShaderObject geo_shd);
    SharedShaderInfo(std::string name, ShaderObject vtx_shd, ShaderObject frg_shad);
    
    std::string      name;
    ShaderObject     vtx_shd;
    ShaderObject     frg_shd;
    GeomShaderObject geo_shd;
    GLint prog_id;
    bool  linked;
    bool  useGeomShader;
    bool  error;
};

/*****************************
 * Shader class              *
 *****************************/

class Shader {
private:
    
    //object represents a single, shared, external resource. Thus all copies
    //reference and share a single representation:
    boost::shared_ptr<SharedShaderInfo> shader_info;
    bool isnull;
	
public:
	Shader(std::string name, std::string vtx_src, std::string frg_src, std::string geo_src, GLint input_type, GLint output_type, int n_generated_vtxs);
	Shader(std::string name, std::string vtx_src, std::string frg_src);
	Shader(); //null shader
	virtual ~Shader();
	
	//read the program sources and attempt to compile the shader, setting the member IDs appropriately.
	//return true if compiled successfully.
	bool compile();
	
	//activate this shader
	//will automatically try to compile the shader if it has not yet done so, unless an auto-compile has already failed.
	//return true if the shader was successfully used, false if the shader could not be compiled/enabled
	bool use();
	
	//disable this shader if it is running. 
	//return true if the shader was running and it was successfully disabled.
	bool disable();
	
	//get the ID of a uniform shader variable by name
	GLint getUniformID(std::string name);
	
	//get the ID of a varying shader variable by name
	GLint getAttribID(std::string name);
	
	//get the name of this shader
	std::string getName();
	
	//get the OpenGL handle for this shader
	//will return 0 if shader not yet compiled
	GLint getProgramID();
	
	//delete all associated shaders and programs
	void release();
	
	//clean up after an error before attempting a re-compile
	//prevents leakage of previously allocated shader handles. 
	void clearError();
	
	//return true if this shader is the stub, do-nothing shader
	//(result of default construction)
	bool isNullShader();
};

//load shader from source files
Shader shaderFromFile(std::string name, std::string filepath);
Shader shaderFromFile(std::string name, std::string filepath, GLint input_geom, GLint output_geom, int generated_vtxs);
Shader shaderFromFile(std::string name, std::string vtx_srcf, std::string geo_srcf, std::string frg_srcf, GLint input_geom, GLint output_geom, int generated_vtxs);

#endif /* SHADER_H_ */
