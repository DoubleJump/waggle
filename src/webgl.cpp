// MESH

static GLuint
convert_update_rate(Buffer_Update_Rate rate)
{
    switch(rate)
    {
        case Buffer_Update_Rate::STATIC:  return GL_STATIC_DRAW;
        case Buffer_Update_Rate::DYNAMIC: return GL_DYNAMIC_DRAW;
        case Buffer_Update_Rate::STREAM:  return GL_STREAM_DRAW;
        NO_DEFAULT_CASE
    }
    return 0;
}

static GLuint 
convert_mesh_primitive(Mesh_Primitive primitive)
{
    switch(primitive)
    {
        case Mesh_Primitive::TRIANGLES: return GL_TRIANGLES;
        case Mesh_Primitive::LINES:     return GL_LINES;
        case Mesh_Primitive::STRIP:     return GL_TRIANGLE_STRIP;
        NO_DEFAULT_CASE
    }
    return 0;
}
/*
static void
set_array_buffer(u32 id)
{
    if(_gl_state.array_buffer == id) return;
    _gl_state.array_buffer = id;
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

static void
set_element_buffer(u32 id)
{
    if(_gl_state.array_buffer == id) return;
    _gl_state.array_buffer = id;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}
*/

static void
bind_buffer(Vertex_Buffer* vb)
{
    u32 vbo = 0;
    glGenBuffers(1, &vbo);
    vb->vbo = vbo;
}

static void
bind_mesh(Mesh* mesh)
{
    u32 vbo = 0;
    u32 ibo = 0;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    mesh->vertex_buffer.vbo = vbo;
    mesh->index_buffer.ibo = ibo;
    //NOTE: very strange bug here if the values are assigned directly;
}
/*
static void 
unbind_mesh(Mesh* mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, 1, GL_STATIC_DRAW);
    glDeleteBuffer(mesh->vertex_buffer.vbo);

    if(mesh_>use_index_buffer)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1, GL_STATIC_DRAW);
        glDeleteBuffer(mesh->index_buffer.ibo);
    }

    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL);
}
*/

static void
update_vertex_buffer(Vertex_Buffer* vb)
{
    auto usage = convert_update_rate(vb->update_rate);
    glBindBuffer(GL_ARRAY_BUFFER, vb->vbo);
    glBufferData(GL_ARRAY_BUFFER, vb->data_size, vb->data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    vb->needs_update = false;
}

/*
static void
update_vertex_buffer_range(Vertex_Buffer* vb, memsize start)
{
    glBindBuffer(GL_ARRAY_BUFFER, vb->id);
    glBufferSubData(GL_ARRAY_BUFFER, start * 4, view);
    glBindBuffer(GL_ARRAY_BUFFER, null);
}
*/

static void
update_index_buffer(Index_Buffer* ib)
{
    auto usage = convert_update_rate(ib->update_rate);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->data_size, ib->data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
    ib->needs_update = false;
}

static void
update_mesh(Mesh* mesh)
{
    if(mesh->vertex_buffer.needs_update)
    {
        update_vertex_buffer(&mesh->vertex_buffer);
    }
    if(mesh->use_index_buffer && mesh->index_buffer.needs_update)
    {
        update_index_buffer(&mesh->index_buffer);
    }
}

static void
link_vertex_attributes(Shader* shader, Vertex_Buffer* vb)
{
    for(auto i = 0; i < shader->num_attributes; ++i)
    {
        auto sa = shader->attributes[i];
        auto va = vb->attributes;

        b32 found = false;
        for(auto j = 0; j < vb->num_attributes; ++j)
        {
            if(va->hash == sa.hash)
            {
                glVertexAttribPointer(sa.location, va->size, GL_FLOAT, va->normalized, vb->stride * sizeof(f32), (void*)(va->offset * sizeof(f32)));
                glEnableVertexAttribArray(sa.location);
                if(vb->instanced)
                {
                    glVertexAttribDivisorANGLE(sa.location, 1);
                }
                found = true;
                break;
            }
            va++;
        }

        /*
        if(!found)
        {
            printf("Shader wants attribute: %s but mesh does not have it.\n", sa.name);
        }
        */
    }
}

static void
draw_mesh(Mesh* mesh)
{
    auto vb = &mesh->vertex_buffer;
    glBindBuffer(GL_ARRAY_BUFFER, vb->vbo);
    link_vertex_attributes(_gl_state.shader, vb);

    auto primitive = convert_mesh_primitive(mesh->primitive);
    if(mesh->use_index_buffer)
    {
        auto ib = &mesh->index_buffer;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->ibo);
        glDrawElements(primitive, ib->count, GL_UNSIGNED_INT, 0);
        //If you're wondering why the 0 - I have no idea...
    }
    else
    {
        glDrawArrays(primitive, 0, vb->count);
    }

    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

static void
draw_mesh_instanced(Mesh* mesh, Vertex_Buffer* instances)
{
    auto shader = _gl_state.shader;
    glBindBuffer(GL_ARRAY_BUFFER, instances->vbo);
    link_vertex_attributes(shader, instances);

    auto vb = &mesh->vertex_buffer;
    glBindBuffer(GL_ARRAY_BUFFER, vb->vbo);
    link_vertex_attributes(shader, vb);

    auto primitive = convert_mesh_primitive(mesh->primitive);
    if(mesh->use_index_buffer)
    {
        auto ib = &mesh->index_buffer;
        //printf("Count: %i\n", instances->count);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->ibo);
        glDrawElementsInstancedANGLE(primitive, ib->count, GL_UNSIGNED_INT, 0, instances->count);
    }
    else
    {
        glDrawArraysInstancedANGLE(primitive, 0, vb->count, instances->count);
    }

    for(auto i = 0; i < shader->num_attributes; ++i)
    {
        auto loc = shader->attributes[i].location;
        glVertexAttribDivisorANGLE(loc, 0);
    }   
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

// SHADER

static GLuint 
compile_shader(const char* src, i32 type)
{
    GLuint shader = glCreateShader(type); 
    glShaderSource(shader, 1, (const GLchar**)&(src), 0);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
    	char error_log[256];
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, &error_log[0]);
        puts("## SHADER COMPILE ERROR ##");
        puts(error_log);
    }

    return shader;
}

static GLuint
link_shader(GLuint vs, GLuint fs)
{
    GLuint id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        char error_log[256];
        GLint log_length = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
        glGetProgramInfoLog(id, log_length, &log_length, &error_log[0]);
        puts("## SHADER LINK ERROR ##");
        puts(error_log);
    }
    
    return id;    
}

static void
bind_shader(Shader* shader, char* vertex_src, char* fragment_src)
{
	auto vs = compile_shader(vertex_src, GL_VERTEX_SHADER);
    auto fs = compile_shader(fragment_src, GL_FRAGMENT_SHADER);
    shader->id = link_shader(vs, fs);

    /*
    glDetachShader(id, vs);
    glDetachShader(id, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
	*/

    char name[SHADER_ATTRIBUTE_NAME_SIZE];
    GLsizei name_length;
    GLint size;
    GLenum type;
    
    glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTES, &shader->num_attributes);
    auto attribute = shader->attributes;
    for(auto i = 0; i < shader->num_attributes; ++i)
    {
        glGetActiveAttrib(shader->id, i, SHADER_ATTRIBUTE_NAME_SIZE, &name_length, &size, &type, &name[0]);

        copy_memory(name, attribute->name, name_length);
        attribute->hash = djb_hash(name);
        //attribute->location = i;
        attribute->location = glGetAttribLocation(shader->id, attribute->name);
        attribute->size = size;
        attribute->type = type;
        attribute++;
    }
    
    i32 sampler_index = 0;
    glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &shader->num_uniforms);

    auto uniform = shader->uniforms;
    for(auto i = 0; i < shader->num_uniforms; ++i)
    {
        glGetActiveUniform(shader->id, i, SHADER_UNIFORM_NAME_SIZE, &name_length, &size, &type, &name[0]);

        copy_memory(name, uniform->name, name_length);
        uniform->hash = djb_hash(name);
        uniform->location = glGetUniformLocation(shader->id, name);
        uniform->size = size;
        uniform->type = type;

        if(uniform->type == GL_SAMPLER_2D)
        {
            uniform->sampler_index = sampler_index;
            sampler_index++;
        }
        uniform++;
    }
}

static void
bind_shader(Shader* shader)
{
    bind_shader(shader, shader->vertex_src, shader->fragment_src);
}

static void 
use_shader(Shader* shader)
{
    //if(_gl_state.shader && shader->id == _gl_state.shader->id) return;
    
    glUseProgram(shader->id);
    _gl_state.shader = shader;
}

static Shader_Uniform*
find_uniform(const char* name)
{
    auto s = _gl_state.shader;
    auto hash = djb_hash(name);
    auto index = 0;
    for(auto i = 0; i < s->num_uniforms; ++i)
    {
        auto uniform = &s->uniforms[i];
        if(uniform->hash == hash)
        {
            return uniform;
        }
    }
    //printf("Uniform not found!!!\n");
    //assert(false, "Uniform %s not found on active shader\n", name);
    return NULL;
}


static void
set_uniform(const char* name, f32 value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    assert(uniform->type == GL_FLOAT, "Target uniform in not a float\n");
    glUniform1f(uniform->location, value);
}

static void
set_uniform(const char* name, f32* value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    assert(uniform->type == GL_FLOAT, "Target uniform in not a float\n");
    glUniform1fv(uniform->location, uniform->size, value);
}


static void
set_uniform(const char* name, i32 value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    assert(uniform->type == GL_INT, "Target uniform in not a int\n");
    glUniform1i(uniform->location, value);
}

static void
set_uniform(const char* name, i32* value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    assert(uniform->type == GL_INT, "Target uniform in not a int\n");
    glUniform1iv(uniform->location, uniform->size, value);
}

static void
set_uniform(const char* name, Vec2 value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    assert(uniform->type == GL_FLOAT_VEC2, "Target uniform in not a vec2\n");
    glUniform2f(uniform->location, value.x, value.y);
}

static void
set_uniform(const char* name, Vec3 value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;

    assert(uniform->type == GL_FLOAT_VEC3, "Target uniform in not a vec3\n");
    glUniform3f(uniform->location, value.x, value.y, value.z);
}

static void
set_uniform(const char* name, Vec4 value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;

    assert(uniform->type == GL_FLOAT_VEC4, "Target uniform in not a vec4\n");
    glUniform4f(uniform->location, value.x, value.y, value.z, value.w);
}

static void
set_uniform(const char* name, Mat3& value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;

    assert(uniform->type == GL_FLOAT_MAT3, "Target uniform in not a mat3\n");
    glUniformMatrix3fv(uniform->location, 1, false, (f32*)&value);
}

static void
set_uniform(const char* name, Mat4& value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;

    assert(uniform->type == GL_FLOAT_MAT4, "Target uniform in not a mat4\n");
    glUniformMatrix4fv(uniform->location, 1, false, (f32*)&value);
}

static void
set_uniform(const char* name, Texture* value)
{
    auto uniform = find_uniform(name);
    if(!uniform) return;
    
    assert(uniform->type == GL_SAMPLER_2D, "Target uniform in not a sampler\n");
    
    auto index = uniform->sampler_index;
    glUniform1i(uniform->location, index);
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, value->id);
}


// TEXTURE

static void
use_texture(Texture* t)
{
    if(_gl_state.texture_id == t->id) return;
    glBindTexture(GL_TEXTURE_2D, t->id);
}

static GLint
convert_texture_filter(Texture_Filter f)
{
    switch(f)
    {
        case Texture_Filter::NEAREST: return GL_NEAREST;
        case Texture_Filter::LINEAR: return GL_LINEAR;
        NO_DEFAULT_CASE
    }
    return 0;
}

static GLint
convert_texture_wrap(Texture_Wrap w)
{
    switch(w)
    {
        case Texture_Wrap::REPEAT: return GL_REPEAT;
        case Texture_Wrap::CLAMP: return GL_CLAMP_TO_EDGE;
        NO_DEFAULT_CASE
    }
    return 0;
}

static GLuint 
convert_texture_size(Texture_Format format)
{
    switch(format)
    {
        case Texture_Format::RGB: return GL_UNSIGNED_BYTE;
        case Texture_Format::RGBA: return GL_UNSIGNED_BYTE;
        case Texture_Format::DEPTH_16: return GL_UNSIGNED_SHORT;
        case Texture_Format::GRAYSCALE: return GL_UNSIGNED_BYTE;
        case Texture_Format::DXT1: return GL_UNSIGNED_BYTE;
        case Texture_Format::DXT5: return GL_UNSIGNED_SHORT;
        NO_DEFAULT_CASE
    }
    return 0;
}

static GLuint 
convert_texture_format(Texture_Format format)
{
    switch(format)
    {
        case Texture_Format::RGB: return GL_RGB;
        case Texture_Format::RGBA: return GL_RGBA;
        case Texture_Format::DEPTH_16: return GL_DEPTH_COMPONENT;
        case Texture_Format::DEPTH_32: return GL_DEPTH_COMPONENT;
        case Texture_Format::GRAYSCALE: return GL_LUMINANCE;
        case Texture_Format::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case Texture_Format::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        NO_DEFAULT_CASE
    }
    return 0;
}

static void
set_sampler(Sampler* sampler)
{
    auto s = convert_texture_wrap(sampler->s);
    auto t = convert_texture_wrap(sampler->t);
    auto u = convert_texture_filter(sampler->up);
    auto d = convert_texture_filter(sampler->down);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, u);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, d); 
    
    if(_gl_extensions.ANISITROPIC_FILTERING)
    {
        f32 max_anisotropy; 
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
        auto anisotropy = clamp(sampler->anisotropy, 0.0f, max_anisotropy);

        glTexParameterf
        (
            GL_TEXTURE_2D, 
            GL_TEXTURE_MAX_ANISOTROPY_EXT, 
            anisotropy
        );
    }    
}

static void
set_sampler(Texture* texture, Sampler* sampler)
{
    use_texture(texture);
    set_sampler(sampler);
    texture->sampler = sampler;
}

static void
bind_texture(Texture* texture)
{
    u32 id = 0;
    glGenTextures(1, &id);  
    texture->id = id;   
}

static void
unbind_texture(Texture* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = 0;
}

static void
update_texture(Texture* t)
{
    //glBindTexture(GL_TEXTURE_2D, t->id);
    
    use_texture(t);
    auto format = convert_texture_format(t->format);
    auto size = convert_texture_size(t->format);
    set_sampler(t->sampler);

    if(t->compressed)
    {
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, t->width, t->height, 0, size, t->data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, t->width, t->height, 0, format, size, t->data);
    }

    /*
    TODO: do this properly
    if(t->num_mipmaps > 0)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    */

    glBindTexture(GL_TEXTURE_2D, NULL);
}

// RENDER TARGET

static void
bind_render_target(Render_Target* t)
{
    glGenFramebuffers(1, &t->frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, t->frame_buffer);

    if(t->color)
    {
        bind_texture(t->color);
        update_texture(t->color);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,t->color->id, 0);   
    }

    if(t->depth)
    {
        bind_texture(t->depth);
        update_texture(t->depth);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,t->depth->id, 0);  
    }  

    if(t->stencil)
    {
        bind_texture(t->depth);
        update_texture(t->depth);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D,t->depth->id, 0); 
    }

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE, "Error creating framebuffer %iu\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

static void
set_render_target(Render_Target* target)
{
    auto fb = target->frame_buffer;
    if(_gl_state.frame_buffer == fb) return;
    _gl_state.frame_buffer = fb;
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    //glBindRenderbuffer(GL_RENDERBUFFER, target->render_buffer);
}

static void
set_default_render_target()
{
    if(_gl_state.frame_buffer == -1) return;
    _gl_state.frame_buffer = -1;
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    //glBindRenderbuffer(GL_RENDERBUFFER, NULL);
}


/*
static void
reset_gl_state()
{
    auto n = _gl_parameters.num_texture_units;
    for(var i = 0; i < n; ++i) 
    {
        GL_activeTexture(GL_TEXTURE0 + i);
        GL_bindTexture(GL_TEXTURE_2D, NULL);
        GL_bindTexture(GL_TEXTURE_CUBE_MAP, NULL);
    }
    glBindBuffer(GL.ARRAY_BUFFER, NULL);
    glBindBuffer(GL.ELEMENT_ARRAY_BUFFER, NULL);
    glBindRenderbuffer(GL.RENDERBUFFER, NULL);
    glBindFramebuffer(GL.FRAMEBUFFER, NULL);

    enable_backface_culling();
    enable_depth_testing();
    set_depth_mode(Depth_Mode::LESS_OR_EQUAL);
    enable_alpha_blending();
    set_blend_mode(Blend_Mode.DEFAULT);
}
*/

static void
set_viewport(Vec4 v)
{
    if(v == _gl_state.view_port) return;
    _gl_state.view_port = v;
    glViewport(v.x, v.y, v.z, v.w);
    glScissor(v.x, v.y, v.z, v.w);
}

static void
set_clear_color(Vec4 c)
{
    glClearColor(c.x, c.y, c.z, c.w);
}

static void
set_clear_mode(i32 mode)
{
    _gl_state.clear_mode = mode;
}

static void
clear_screen()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(_gl_state.clear_mode);
}


// CULLING

static void
enable_scissor_test()
{
    if(_gl_state.scissor_test) return;
    _gl_state.scissor_test = true;
    glEnable(GL_SCISSOR_TEST);
}

static void
disable_scissor_test()
{
    if(!_gl_state.scissor_test) return;
    _gl_state.scissor_test = false;
    glDisable(GL_SCISSOR_TEST);
}

static void
enable_backface_culling()
{
    if(_gl_state.face_culling) return;
    _gl_state.face_culling = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

static void
disable_backface_culling()
{
    if(!_gl_state.face_culling) return;
    _gl_state.face_culling = false;
    glDisable(GL_CULL_FACE);
}

static void
set_winding_order(Winding_Order order)
{
    if(_gl_state.winding_order == order) return;
    _gl_state.winding_order = order;
    switch(order)
    {
        case Winding_Order::CW:
        {
            glFrontFace(GL_CW);
            break;
        }
        case Winding_Order::CCW:
        {
            glFrontFace(GL_CCW);
            break;
        }
    }
}

// DEPTH TESTING

static void 
enable_depth_testing()
{
    if(_gl_state.depth_test) return;
    _gl_state.depth_test = true;
    glEnable(GL_DEPTH_TEST);
}

static void
disable_depth_testing()
{
    if(!_gl_state.depth_test) return;
    _gl_state.depth_test = false;
    glDisable(GL_DEPTH_TEST);
}

static void
enable_depth_writing()
{
    if(_gl_state.depth_write) return;
    _gl_state.depth_write = true;
    glDepthMask(true);
}

static void
disable_depth_writing()
{
    if(!_gl_state.depth_write) return;
    _gl_state.depth_write = false;
    glDepthMask(false);
}

static void
set_depth_range(f32 min, f32 max)
{
    auto range = _gl_state.depth_range;
    if(range.x == min && range.y == max) return;
    glDepthRangef(min, max);
    _gl_state.depth_range = {min, max};
}

static void
set_depth_mode(Depth_Mode mode)
{
    if(_gl_state.depth_mode == mode) return;
    _gl_state.depth_mode = mode;

    switch(mode)
    {
        case Depth_Mode::LESS:
        {
            glDepthFunc(GL_LESS);
            break;
        }
        case Depth_Mode::LESS_OR_EQUAL:
        {
            glDepthFunc(GL_LEQUAL);
            break;
        }
        case Depth_Mode::GREATER:
        {
            glDepthFunc(GL_GREATER);
            break;
        }
        case Depth_Mode::GREATER_OR_EQUAL:
        {
            glDepthFunc(GL_GEQUAL);
            break;
        }
        case Depth_Mode::EQUAL:
        {
            glDepthFunc(GL_EQUAL);
            break;
        }
        case Depth_Mode::NOT_EQUAL:
        {
            glDepthFunc(GL_NOTEQUAL);
            break;
        }
        case Depth_Mode::ALWAYS:
        {
            glDepthFunc(GL_ALWAYS);
            break;
        }
        case Depth_Mode::NEVER:
        {
            glDepthFunc(GL_NEVER);
            break;
        }
        NO_DEFAULT_CASE
    }   
}

// ALPHA BLENDING

static void
enable_alpha_blending()
{
    if(_gl_state.alpha_blending) return;
    _gl_state.alpha_blending = true;
    glEnable(GL_BLEND);
}

static void
disable_alpha_blending()
{
    if(!_gl_state.alpha_blending) return;
    _gl_state.alpha_blending = false;
    glDisable(GL_BLEND);
}

static void
set_blend_mode(Blend_Mode mode)
{
    if(mode == _gl_state.blend_mode) return;
    _gl_state.blend_mode = mode;

    switch(mode)
    {
        case Blend_Mode::DEFAULT:
        {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            /*
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
            */

            break;
        }
        case Blend_Mode::ADD:
        {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        case Blend_Mode::DARKEN:
        {
            glBlendEquation(GL_FUNC_SUBTRACT);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }
        case Blend_Mode::LIGHTEN:
        {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }
        case Blend_Mode::DIFFERENCE:
        {
            glBlendEquation(GL_FUNC_SUBTRACT);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }
        case Blend_Mode::MULTIPLY:
        {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        }
        case Blend_Mode::SCREEN:
        {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            break;
        }
        case Blend_Mode::INVERT:
        {
            //glBlendEquation(GL_FUNC_ADD);
            //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR); 

            glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            break;
        }
        case Blend_Mode::OVERLAY:
        {
            break;
        }
        NO_DEFAULT_CASE
    }
}

static GL_State*
init_webgl() //params
{
    //emscripten_set_canvas_size(width, height);
    EmscriptenWebGLContextAttributes attributes;
    attributes.alpha = false; //set true to blend with underlying webpage
    attributes.depth = true;
    attributes.stencil = true;
    attributes.antialias = false;
    attributes.premultipliedAlpha = false;
    attributes.preserveDrawingBuffer = false;
    attributes.preferLowPowerToHighPerformance = false;
    attributes.failIfMajorPerformanceCaveat = false;
    attributes.majorVersion = 1;
    attributes.minorVersion = 0;
    attributes.enableExtensionsByDefault = false;

    auto ctx = emscripten_webgl_create_context(NULL, &attributes);
    assert(ctx, "Webgl ctx could not be created\n");
    emscripten_webgl_make_context_current(ctx);

    const char* extensions[] = 
    {
        "ANGLE_instanced_arrays",
        "EXT_blend_minmax",
        "EXT_frag_depth",
        "EXT_shader_texture_lod",
        "EXT_texture_filter_anisotropic",
        "OES_element_index_uint",
        "OES_standard_derivatives",
        "OES_texture_float",
        "OES_texture_float_linear",
        "OES_texture_half_float",
        "OES_texture_half_float_linear",
        "OES_vertex_array_object",
        "WEBGL_compressed_texture_s3tc",
        "WEBGL_debug_renderer_info",
        "WEBGL_debug_shaders",
        "WEBGL_depth_texture",
        "WEBGL_draw_buffers",
        "WEBGL_lose_context",
    };

    // TODO sign these to the _gl_extensions struct
    for(auto i = 0; i < ARRAY_COUNT(extensions); ++i)
    {
        auto supported = emscripten_webgl_enable_extension(ctx, extensions[i]);
    }

    // STATE CACHING

    _gl_state.ctx = ctx;
    _gl_state.shader = NULL;
    _gl_state.array_buffer = -1;
    _gl_state.element_buffer = -1;
    _gl_state.frame_buffer = -1;
    _gl_state.render_buffer = -1;
    _gl_state.clear_mode = Clear_Mode::COLOR | Clear_Mode::DEPTH;
    _gl_state.clear_color = {0,0,0,1};
    _gl_state.alpha_blending = true;
    _gl_state.blend_mode = Blend_Mode::DEFAULT;
    _gl_state.scissor_test = true;
    _gl_state.face_culling = true;
    _gl_state.winding_order = Winding_Order::CCW;
    _gl_state.depth_test = true;
    _gl_state.depth_write = true;
    //_gl_state.depth_mode = Depth_Mode::GREATER_OR_EQUAL;
    _gl_state.depth_mode = Depth_Mode::LESS_OR_EQUAL;
    _gl_state.depth_range = {-1,1};
    _gl_state.view_port = {0,0,0,0};
    _gl_state.texture_id = -1;

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDepthFunc(GL_GEQUAL);

    glDepthMask(true);
    glColorMask(true, true, true, true);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_SCISSOR_TEST);
    
    //glClearColor(0,0,0,1);
    //glDepthRangef(-1.0f, 1.0f);

    return &_gl_state;
}