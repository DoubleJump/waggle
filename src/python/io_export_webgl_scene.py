# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

bl_info = {
	"name": "Export GBLIB Scene(.scene)",
	"author": "Gareth Battensby",
	"version": (1, 0),
	"blender": (2, 6, 0),
	"location": "File > Export > GBLIB (.scene)",
	"description": "Export GBLIB Binary (.scene)",
	"warning": "",
	"wiki_url": "",
	"tracker_url": "",
	"category": "Import-Export"}

import bpy
from bpy.props import *
import mathutils, struct
from math import radians
from bpy_extras.io_utils import ExportHelper
from struct import pack, unpack

ANIM_TYPE_ENTITY = 0
ANIM_TYPE_MATERIAL = 1
ANIM_TYPE_ARMATURE = 2

OB_TYPE_CAMERA = 4
OB_TYPE_LAMP = 5
OB_TYPE_MESH = 6
OB_TYPE_MATERIAL = 7
OB_TYPE_ACTION = 8
OB_TYPE_OBJECT = 9
OB_TYPE_EMPTY = 10
OB_TYPE_ARMATURE = 11
OB_TYPE_ARMATURE_ACTION = 12
OB_TYPE_CURVE = 13
OB_TYPE_FILE_END = -1

FLOAT_TYPE_F32 = 0
FLOAT_TYPE_VEC2 = 1
FLOAT_TYPE_VEC3 = 2
FLOAT_TYPE_VEC4 = 3
FLOAT_TYPE_QUATERNION = 4

GB_MATRIX = mathutils.Matrix.Rotation(radians(-90.0), 4, 'X')

def round_vec3(v, precision):
	return round(v[0], precision), round(v[1], precision), round(v[2], precision)

def round_vec2(v, precision):
	return round(v[0], precision), round(v[1], precision)

class VertexAttribute:
	def __init__(self, name, size, normalized):
		self.name = name
		self.size = size
		self.norm = normalized

class FileWriter:
	def __init__(self, path):
		self.target = open(path, "wb")
		self.offset = 0
		self.BYTE_ALIGNMENT = 4

	def close(self):
		self.target.close()

	def padding(self, size):
		return (self.BYTE_ALIGNMENT - size % self.BYTE_ALIGNMENT) % self.BYTE_ALIGNMENT

	def i32(self, val):
		self.target.write(pack("i", val))
		self.offset += 4

	def u32(self, val):
		self.target.write(pack("I", val))
		self.offset += 4

	def f64(self, val):
		self.target.write(pack("d", val))
		self.offset += 8

	def f32(self, val):
		self.target.write(pack("f", val))
		self.offset += 4

	def b32(self, val):
		if val is True: self.i32(1)
		else: self.i32(0) 

	def vec3(self, val):
		self.f32(val.x)
		self.f32(val.y)
		self.f32(val.z)

	def quaternion(self, val):
		self.f32(val.x)
		self.f32(val.y)
		self.f32(val.z)
		self.f32(val.w)

	def mat4(self, val):
		matrix = val.transposed()
		for i in range(0,4):
			for j in range(0,4):
				self.f32(matrix[i][j])

	'''
	def string(self, val):
		str_encoded = val.lower().encode('ascii')
		str_len = len(str_encoded)
		new_offset = self.offset + str_len
		padding = self.BYTE_PADDING - (new_offset % self.BYTE_PADDING);
		self.i32(padding)
		self.i32(str_len)
		self.target.write(str_encoded)
		self.offset += str_len
		for i in range(0, padding):
			self.target.write(pack('B', 0))
			self.offset += 1	
	'''

	def string(self, val):
		str_encoded = val.lower().encode('ascii')
		str_encoded += '\0'.encode('ascii')
		size = len(str_encoded)
		pad = self.padding(size)

		self.i32(size)
		self.target.write(str_encoded)
		for i in range(0, pad):
			self.target.write(pack("x"))
		self.offset += size + pad

	'''
	def bytes(self, val):
		self.target.write(val)
		self.offset += len(val)
	'''
	def transform(self, transform):
		if(transform.parent == None): self.string("none")
		else: self.string(transform.parent.name) 
		transform.rotation_mode = 'QUATERNION'
		self.vec3(transform.location)
		self.vec3(transform.scale)
		self.quaternion(transform.rotation_quaternion)

	def material(self, ctx, material):
		if ctx.export_materials is False: return
		print("Exporting Material: " + material.name)
		self.i32(OB_TYPE_MATERIAL)
		self.string(material.name)
		self.string(material['shader'])

		num_textures = 0
		for i in material.texture_slots:
			if i == None: continue
			texture = bpy.data.textures[i.name]
			if texture.image == None: continue
			num_textures += 1

		self.i32(num_textures)
		
		for i in material.texture_slots:
			if i == None: continue
			texture = bpy.data.textures[i.name]
			if texture.image == None: continue
			self.string(texture.image.name.split('.')[0])
			self.string(texture['sampler'])

	def empty(self, ctx, empty):
		if ctx.export_entities is False: return
		print("Exporting Empty: " + empty.name)
		self.i32(OB_TYPE_EMPTY)
		self.string(empty.name)
		self.transform(empty)

	def object(self, ctx, ob):
		if ctx.export_entities is False: return
		print("Exporting Object: " + ob.name)
		self.i32(OB_TYPE_OBJECT)
		self.string(ob.name)
		self.transform(ob)
		if(len(ob.material_slots) > 0 and ctx.export_materials is True):
			self.string(ob.material_slots[0].material.name) #our material
		else:
			self.string('none')
		self.string(ob.data.name) #our mesh

	def lamp(self, ctx, ob, lamp):
		if ctx.export_lamps is False: return
		print("Exporting Lamp: " + ob.name)
		self.i32(OB_TYPE_LAMP)
		self.string(ob.name)
		self.transform(ob)
		self.f32(lamp.energy)
		self.f32(lamp.distance)

	def curve(self, ctx, ob, curve):
		if ctx.export_curves is False: return
		print("Exporting Curve: " + ob.name)
		self.i32(OB_TYPE_CURVE)
		self.string(ob.name)
		self.b32(ctx.curve_2d)
		#self.i32(curve.resolution_u)
		points = []

		num_bezier_points = 0
		for spline in curve.splines:
			num_bezier_points += len(spline.bezier_points)

		self.i32(num_bezier_points)
		
		for spline in curve.splines:
			if ctx.curve_2d is True:
				for point in spline.bezier_points:
					left = round_vec2(point.handle_left, ctx.curve_precision)
					co = round_vec2(point.co, ctx.curve_precision)
					right = round_vec2(point.handle_right, ctx.curve_precision)

					self.f32(left[0])
					self.f32(left[1])
					self.f32(co[0])
					self.f32(co[1])
					self.f32(right[0])
					self.f32(right[1])

			else:
				for point in spline.bezier_points:
					left = round_vec3(point.handle_left, ctx.curve_precision)
					co = round_vec3(point.co, ctx.curve_precision)
					right = round_vec3(point.handle_right, ctx.curve_precision)
						
					self.f32(left[0])
					self.f32(left[1])
					self.f32(left[2])
					self.f32(co[0])
					self.f32(co[1])
					self.f32(co[2])
					self.f32(right[0])
					self.f32(right[1])
					self.f32(right[2])


	def camera(self, ctx, ob, camera):
		if ctx.export_cameras is False: return
		print("Exporting Camera: " + ob.name)
		self.i32(OB_TYPE_CAMERA)
		self.string(ob.name)
		self.transform(ob)
		if camera.type == 'PERSP': 
			self.i32(1)
			self.f32(1)
		else: 
			self.i32(0)
			self.f32(camera.ortho_scale)
		self.f32(camera.clip_start)
		self.f32(camera.clip_end)
		self.f32(camera.lens)

	def armature(self, ctx, ob):
		if ctx.export_armatures is False: return
		print("Exporting Armature: " + ob.name)

		self.i32(OB_TYPE_ARMATURE)
		self.string(ob.name)

		armature_ob = bpy.data.objects[ob.name]
		armature_data = bpy.data.armatures[ob.name]
		num_bones = len(armature_data.bones)
		self.i32(num_bones)

		index = 0
		bone_ids = {}
		for bone in armature_data.bones:

			local_matrix = bone.matrix_local
			world_matrix = bone.matrix_local

			if bone.parent is None:
				self.i32(-1)
			else:
				self.i32(bone_ids[bone.parent.name])
				local_matrix = bone.parent.matrix_local.inverted() * bone.matrix_local

			self.mat4(local_matrix)
			self.mat4(world_matrix.inverted()) #inverse bind pose

			bone_ids[bone.name] = index
			index += 1
		
	def mesh(self, ctx, ob, scene):
		print("Exporting Mesh: " + ob.data.name)

		# up_axis('Y')

		modifiers = ob.modifiers
		has_triangulate = False
		for mod in modifiers:
			if(mod.type == 'TRIANGULATE'):
				has_triangulate = True

		if not has_triangulate:
			modifiers.new("TEMP", type = 'TRIANGULATE')

		mesh = ob.to_mesh(scene = scene, apply_modifiers = True, settings = 'PREVIEW')

		armature = None
		modifiers = ob.modifiers
		for mod in modifiers:
			if(mod.type == 'ARMATURE'):
				armature = mod.object

		vertex_map = {}
		vertex_buffer = []
		index_buffer = []
		vertex_count = 0

		#mesh.transform(matrix)
		mesh.calc_normals()
		mesh.calc_tessface() 

		# Create a list for each vertex color layer
		color_layers = mesh.tessface_vertex_colors
		num_colors = ctx.color_channels
		if num_colors < 1: num_colors = 1
		elif num_colors > 4: num_colors = 4

		# Create a list for each uv layer
		uv_layers = mesh.tessface_uv_textures

		for i, f in enumerate(mesh.tessfaces): # For each face on mesh
			for j, v in enumerate(f.vertices): # For each vertex of face
			
				p = round_vec3(mesh.vertices[v].co, ctx.vertex_precision)
				norm = round_vec3(mesh.vertices[v].normal, ctx.normal_precision)

				if ctx.export_indices:
					key = p,norm
					index = vertex_map.get(key)
					if not index is None:
						index_buffer.append(index)
						continue
					else:
						vertex_map[key] = vertex_count

				t_pos = round_vec3(GB_MATRIX * mesh.vertices[v].co, ctx.vertex_precision)

				t_norm = round_vec3(GB_MATRIX * mesh.vertices[v].normal, ctx.normal_precision)

				vertex_buffer.append(t_pos[0])
				vertex_buffer.append(t_pos[1])
				if ctx.export_2d is False:
					vertex_buffer.append(t_pos[2])

				if ctx.export_normals:
					vertex_buffer.append(t_norm[0])
					vertex_buffer.append(t_norm[1])
					vertex_buffer.append(t_norm[2])

				if ctx.export_uvs:
					for n, l in enumerate(uv_layers):
						uv = uv_layers[n].data[i].uv[j]
						uv = round_vec2(uv, ctx.uv_precision)
						vertex_buffer.append(uv[0])
						vertex_buffer.append(uv[1])

				if ctx.export_vertex_colors:
					for k, l in enumerate(color_layers):
						c = None;
						if(j == 0): c = color_layers[k].data[i].color1
						elif(j == 1): c = color_layers[k].data[i].color2
						elif(j == 2): c = color_layers[k].data[i].color3
						else: c = color_layers[k].data[i].color4
						c = round_vec3(c, ctx.color_precision);

						vertex_buffer.append(c[0])
						if num_colors > 1: vertex_buffer.append(c[1])
						if num_colors > 2: vertex_buffer.append(c[2])
						if num_colors > 3: vertex_buffer.append(1.0)
																						  
				index_buffer.append(vertex_count)

				if armature and ctx.export_armatures and ctx.export_weights:
					groups = mesh.vertices[v].groups
					num_groups = len(groups)
					if num_groups is 0:
						vertex_buffer.append(0)
						vertex_buffer.append(0.5)
						vertex_buffer.append(0)
						vertex_buffer.append(0.5)
					elif num_groups is 1:
						for g in groups:
							group_index = 0
							vertex_group = ob.vertex_groups[g.group]
							for b in armature.pose.bones:
								if b.name == vertex_group.name: 
									vertex_buffer.append(group_index)
									vertex_buffer.append(g.weight)
									break
								group_index += 1

						vertex_buffer.append(0)
						vertex_buffer.append(0)
					else:
						matches = 0
						for g in groups:
							group_index = 0
							vertex_group = ob.vertex_groups[g.group]
							for b in armature.pose.bones:
								if b.name == vertex_group.name: 
									vertex_buffer.append(group_index)
									vertex_buffer.append(g.weight)
									matches += 1
									break
								group_index += 1

							#vertex_buffer.append(group_index)
							#vertex_buffer.append(g.weight)
							if matches == 2: break

				vertex_count += 1

		attributes = []
		if ctx.export_2d:
			attributes.append(VertexAttribute('position', 2, False))
		else:
			attributes.append(VertexAttribute('position', 3, False))

		if ctx.export_normals:
			attributes.append(VertexAttribute('normal', 3, False))

		if ctx.export_uvs:
			uv_ln = len(uv_layers)
			for i in range(0, uv_ln):
				if i is 0:
					attributes.append(VertexAttribute('uv', 2, False))
				else:
					attributes.append(VertexAttribute('uv' + str(i+1), 2, False))
		
		if ctx.export_vertex_colors:
			color_ln = len(color_layers)
			for i in range(0, color_ln):
				if i is 0:
					attributes.append(VertexAttribute('color', num_colors, True))
				else:
					attributes.append(VertexAttribute('color' + str(i+1), num_colors, True))

		if ctx.export_weights:
			weight_ln = len(ob.vertex_groups)
			if weight_ln > 0:
				attributes.append(VertexAttribute('weight', 4, False))

		self.i32(OB_TYPE_MESH)
		self.string(ob.data.name)

		self.i32(len(vertex_buffer))
		for vertex in vertex_buffer:
			self.f32(vertex)

		if ctx.export_indices:
			self.i32(len(index_buffer))
			for index in index_buffer:
				self.i32(index)
		else: self.i32(0)

		self.i32(len(attributes))
		for attr in attributes:
			self.string(attr.name)
			self.i32(attr.size)
			self.b32(attr.norm)

		bpy.data.meshes.remove(mesh)
		if not has_triangulate:
			modifiers.remove(modifiers[len(modifiers)-1])

	def animation_curve(self, curve, fps):
		self.i32(len(curve.keyframe_points))
		for keyframe in curve.keyframe_points:
			self.f32(keyframe.handle_left[0] / fps)
			self.f32(keyframe.handle_left[1])
			self.f32(keyframe.co[0] / fps)
			self.f32(keyframe.co[1])
			self.f32(keyframe.handle_right[0] / fps)
			self.f32(keyframe.handle_right[1])

	def action(self, ctx, action, ob, owner, scene, anim_type):
		if ctx.export_actions is False: return

		print("Exporting Action: " + action.name)

		self.i32(OB_TYPE_ACTION)
		self.i32(anim_type)
		self.string(action.name)
		self.string(ob.name) #target
		self.i32(len(action.fcurves))

		last_prop = ""
		props = []
		types = []
		offsets = []

		num_props = 0
		offset = 0
		for curve in action.fcurves:
			prop = curve.data_path
			if prop != last_prop:
				num_props += 1
				
				if prop == 'location': 
					offsets.append(offset)
					offset+=3
					props.append("position")
					types.append(FLOAT_TYPE_VEC3)
				elif prop == 'rotation_euler':
					offsets.append(offset)
					offset+=3
					props.append("rotation") 
					types.append(FLOAT_TYPE_VEC3)
				elif prop == 'rotation_quaternion':
					offsets.append(offset)
					offset+=4
					props.append("rotation")
					types.append(FLOAT_TYPE_QUATERNION)
					
				last_prop = prop

		self.i32(num_props)
		for i in range(0, num_props):
			self.string(props[i])
			self.i32(types[i])
			self.i32(offsets[i])

		for curve in action.fcurves:
			self.animation_curve(curve, scene.render.fps)

	def armature_action(self, ctx, action, ob, scene):
		if ctx.export_actions is False: return
		print("Exporting Rig Action: " + action.name)

		self.i32(OB_TYPE_ARMATURE_ACTION)
		self.string(action.name)
		self.i32(len(action.fcurves))

		for curve in action.fcurves:
			path = curve.data_path
			prop = path.split('.')[2]
			target_bone = path[12:].split('"')[0]

			bone_index = 0
			for bone in ob.pose.bones:
				if bone.name == target_bone: break
				bone_index += 1
			self.i32(bone_index)

			data_type = ob.path_resolve(path)
			value_mode = 1
			if prop == 'location': prop = 'position'
			elif prop == 'rotation_euler': 
				prop = 'rotation'
			elif prop == 'rotation_quaternion':
				prop = 'rotation'
				value_mode = 2
			self.string(prop)

			index = curve.array_index
			if value_mode == 2:
				if index == 0: index = 3
				elif index == 1: index = 0 
				elif index == 2: index = 1
				elif index == 3: index = 2 
			self.i32(index)

			self.animation_curve(curve, scene.render.fps)

class ExportTest(bpy.types.Operator, ExportHelper):
	bl_idname = "export_gblib.test"
	bl_label = "Export GBLIB (.scene)"

	filename_ext = ".scene"
	filter_glob = StringProperty(default = "*.scene", options = {'HIDDEN'})

	ui_tab = EnumProperty(
            items=(('MAIN', "Main", "Main basic settings"),
                   ('MESH', "Mesh", "Mesh related settings"),
                   ('CURVE', "Curve", "Curve related settings")
                   ),
            name="ui_tab",
            description="Import options categories",
            )

	vertex_precision = IntProperty(name="Vertex Precision", description="Vertex precision", default=3)
	normal_precision = IntProperty(name="Normal Precision", description="Normal precision", default=3)
	uv_precision = IntProperty(name="UV Precision", description="UV precision", default=3)
	color_precision = IntProperty(name="Color Precision", description="Color precision", default=3)
	color_channels = IntProperty(name="Color Channels", description="Color channels", default=4)

	export_indices = BoolProperty(name="Export Indices", description="Export index buffer", default=False)
	export_2d = BoolProperty(name="Export 2D", description="Only export X and Y coordinates", default=False)
	export_normals = BoolProperty(name="Export Normals", description="Include normals as vertex attribute", default=True)
	export_uvs = BoolProperty(name="Export Uvs", description="Include uvs as vertex attribute", default=True)
	export_vertex_colors = BoolProperty(name="Export Vertex Colors", description="Include colors as vertex attribute", default=True)
	export_weights = BoolProperty(name="Export Weights", description="Include weights as vertex attribute", default=True)

	export_materials = BoolProperty(name="Export Materials", description="Include materials", default=True)
	export_entities = BoolProperty(name="Export Entities", description="Include entities", default=True)
	export_animations = BoolProperty(name="Export Animations", description="Include materials", default=True)
	export_cameras = BoolProperty(name="Export Cameras", description="Include cameras", default=True)
	export_lamps = BoolProperty(name="Export Lamps", description="Include lamps", default=True)
	export_curves = BoolProperty(name="Export Curves", description="Include curves", default=True)
	export_armatures = BoolProperty(name="Export Armatures", description="Include armatures", default=True)
	export_actions = BoolProperty(name="Export Actions", description="Include actions", default=True)

	curve_2d = BoolProperty(name="2D Curves", description="Only export curve xy coords", default=False)
	curve_precision = IntProperty(name="Curve Precision", description="Curve precision", default=3)

	def draw(self, context):
		layout = self.layout

		layout.prop(self, "ui_tab", expand=True)
		if self.ui_tab == 'MAIN':
			layout.prop(self, "export_materials")
			layout.prop(self, "export_entities")
			layout.prop(self, "export_animations")
			layout.prop(self, "export_cameras")
			layout.prop(self, "export_lamps")
			layout.prop(self, "export_curves")
			layout.prop(self, "export_armatures")
			layout.prop(self, "export_actions")
		elif self.ui_tab == 'MESH':
			layout.prop(self, "vertex_precision")
			layout.prop(self, "normal_precision")
			layout.prop(self, "uv_precision")
			layout.prop(self, "color_precision")
			layout.prop(self, "color_channels")
			layout.prop(self, "export_indices")
			layout.prop(self, "export_2d")
			layout.prop(self, "export_normals")
			layout.prop(self, "export_uvs")
			layout.prop(self, "export_vertex_colors")
			layout.prop(self, "export_weights")
		elif self.ui_tab == 'CURVE':
			layout.prop(self, "curve_2d")
			layout.prop(self, "curve_precision")
		
	def execute(self, context):
			
		scene = bpy.context.scene 
		scene.frame_set(0)

		filepath = self.filepath
		filepath = bpy.path.ensure_ext(filepath, self.filename_ext)
		writer = FileWriter(filepath)

		print(self.export_normals)

		exported_meshes = []
		exported_actions = []
		exported_materials = []
		exported_cameras = []
		exported_lamps = []
		exported_armatures = []
		exported_curves = []

		print("")
		print("######### EXPORTING SCENE: " + scene.name + " ###########")
		print("")

		scene.objects.active = None;

		for ob in scene.objects:
			
			if ob.type == 'ARMATURE':
				armature = bpy.data.objects[ob.name]
				if not armature in exported_armatures:
					action = self.find_action(armature, exported_actions)
					if not action is None:
						writer.armature_action(self, action, ob, scene)
						exported_actions.append(action)

					writer.armature(self, ob)
					exported_armatures.append(armature)
			else:
				action = self.find_action(ob, exported_actions)
				if not action is None:
					writer.action(self, action, ob, ob, scene, ANIM_TYPE_ENTITY)
					exported_actions.append(action)

			if ob.type == 'CAMERA': 
				camera = ob.data
				if not camera in exported_cameras:
					action = self.find_action(camera, exported_actions)
					if not action is None:
						writer.action(self, action, ob, ob, scene, ANIM_TYPE_ENTITY)
						exported_actions.append(action)

					writer.camera(self, ob, camera)
					exported_cameras.append(camera)

			elif ob.type == 'CURVE' and self.export_curves is True:
				curve = ob.data
				if not curve in exported_curves:
					action = self.find_action(curve, exported_actions)
					if not action is None:
						writer.action(self, action, ob, ob, scene, ANIM_TYPE_ENTITY)
						exported_actions.append(action)
							
					writer.curve(self, ob, curve)
					exported_cameras.append(curve)

			elif ob.type == 'LAMP': 
				lamp = ob.data
				if not lamp in exported_lamps:
					action = self.find_action(lamp, exported_actions)
					if not action is None:
						writer.action(self, action, ob, ob, scene, ANIM_TYPE_ENTITY)
						exported_actions.append(action)
				
					writer.lamp(self, ob, lamp)
					exported_lamps.append(lamp)

			elif ob.type == 'MESH':
				if len(ob.material_slots) > 0:
					material = ob.material_slots[0].material
					if not material in exported_materials:
						action = self.find_action(material, exported_actions)
						if not action is None:
							writer.action(action, ob, material, scene, ANIM_TYPE_MATERIAL)
							exported_actions.append(action)

						writer.material(self, material)
						exported_materials.append(material)

				if not ob.data.name in exported_meshes:
					writer.mesh(self, ob, scene)
					exported_meshes.append(ob.data.name)

				writer.object(self, ob)

			elif ob.type == 'EMPTY':
				writer.empty(self, ob)

		writer.i32(OB_TYPE_FILE_END)
		writer.close()
		#print(writer.offset)

		print("")
		print("######### EXPORT COMPLETED ###########")
		print("")
							
		return {'FINISHED'}

	def find_action(self, ob, actions):
		if ob.animation_data is None: return None
		action = ob.animation_data.action
		if action in actions: return None
		return action

	def invoke(self, context, event):
		wm = context.window_manager

		if True:
			wm.fileselect_add(self) # will run self.execute()
			return {'RUNNING_MODAL'}
		elif True:
			wm.invoke_search_popup(self)
			return {'RUNNING_MODAL'}
		elif False:
			return wm.invoke_props_popup(self, event)
		elif False:
			return self.execute(context)


### REGISTER ###

def menu_func(self, context):
	self.layout.operator(ExportTest.bl_idname, text="GBLIB (.scene)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)
	
if __name__ == "__main__":
	register()