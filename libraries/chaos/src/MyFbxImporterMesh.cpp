#include <chaos/MyFbxImporter.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/GPUSimpleMesh.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/Buffer.h>
#include <chaos/GLTools.h>
#include <chaos/GPUDrawPrimitive.h>

namespace chaos
{




#if 0




	// --------------------------------------------
		else if (type == FbxDeformer::EDeformerType::eBlendShape)
		{
			FbxBlendShape * blend_shape = (FbxBlendShape*)geometry;
			if (blend_shape != nullptr)
			{
				int channel_count = blend_shape->GetBlendShapeChannelCount();
				for (int j = 0; j < channel_count; ++j)
				{
					FbxBlendShapeChannel * channel = blend_shape->GetBlendShapeChannel(j);
					if (channel != nullptr)
					{
						int      shape_count = channel->GetTargetShapeCount();
						double * weights = channel->GetTargetShapeFullWeights();
						for (int k = 0; k < shape_count; ++k)
						{
							FbxShape * shape = channel->GetTargetShape(k);
							if (shape != nullptr)
							{
								int   ctrl_point_count = shape->GetControlPointIndicesCount();
								int * ctrl_point_indices = shape->GetControlPointIndices();

							}
						}
					}
				}
			}






			return true;
		}

#endif

		MyFbxImporter::BoneTableType MyFbxImporter::GetBoneData(FbxMesh * mesh)
		{
			BoneTableType result; // control_point_index => array of [cluster_index, weight]

			int skinning_count = mesh->GetDeformerCount(FbxDeformer::eSkin);

			for (int i = 0; i < skinning_count; ++i)
			{
				FbxSkin * skin = (FbxSkin *)mesh->GetDeformer(i, FbxDeformer::eSkin);
				if (skin == nullptr)
					continue;

				FbxSkin::EType skin_type = skin->GetSkinningType(); // eRigid, eLinear, eDualQuaternion, eBlend 

				int cluster_count = skin->GetClusterCount();
				for (int j = 0; j < cluster_count; ++j) // get all bones for that skinning
				{
					FbxCluster * cluster = skin->GetCluster(j);
					if (cluster == nullptr)
						continue;

					FbxNode * node = cluster->GetLink();

					FbxSkeleton * Skel = node->GetSkeleton();

					LogTools::Log("CLUSTER node = [%08x] node = %s [%s], cluster = %s", node, node->GetName(), node->GetTypeName(), cluster->GetName());

					int      ctrl_point_count = cluster->GetControlPointIndicesCount();
					int *    ctrl_point_indices = cluster->GetControlPointIndices();
					double * ctrl_point_weight = cluster->GetControlPointWeights();

					for (int k = 0; k < ctrl_point_count; ++k)
						if (ctrl_point_weight[k] > 0.0)
							result[ctrl_point_indices[k]].push_back(std::make_pair((int32_t)j, (float)ctrl_point_weight[k]));
				}
			}
			return result;
		}

		size_t MyFbxImporter::GetMaxBoneCount(MyFbxImporter::BoneTableType const & table)
		{
			size_t result = 0;
			for (auto const & entry : table)
				result = std::max(result, entry.second.size());
			return result;
		}

		/** read an element in buffer (direct or indirect reading) */
		template<typename T>
		static T ReadFbxElement(FbxLayerElementTemplate<T> const * element_array, int index)
		{
			FbxLayerElement::EReferenceMode mode = element_array->GetReferenceMode();

			if (mode == FbxGeometryElement::eDirect)
			{
				return element_array->GetDirectArray().GetAt(index); // XXX : GetAt() is safe for vertex out of bound
			}
			else if (mode == FbxGeometryElement::eIndex || mode == FbxGeometryElement::eIndexToDirect)
			{
				return element_array->GetDirectArray().GetAt(
					element_array->GetIndexArray().GetAt(index)
				);
			}

			return T(); // default result (should never happen)
		}

		/** test whether the element is valid */
		template<typename T>
		static bool IsGeometryElementValid(T const * element)
		{
			if (element != nullptr)
			{
				FbxLayerElement::EMappingMode mode = element->GetMappingMode();
				if (
					mode == FbxLayerElement::EMappingMode::eAllSame ||
					mode == FbxLayerElement::EMappingMode::eByControlPoint ||
					mode == FbxLayerElement::EMappingMode::eByPolygon ||
					mode == FbxLayerElement::EMappingMode::eByPolygonVertex)
					return true;
			}
			return false;
		}

		/** if element is valid, this function insert a new GPUVertexDeclarationEntry in the declaration */
		template<typename T>
		static void UpdateVertexDeclarationWithGeometryElement(T const * element, GPUVertexDeclaration & vertex_declaration, VertexAttributeSemantic semantic, int semantic_index, VertexAttributeType type)
		{
			if (IsGeometryElementValid(element))
				vertex_declaration.Push(semantic, semantic_index, type);
		}

		/** conversion methods : in our use cases, FbxVector2 become a vec2 (W component is unused) : used for UV */
		static glm::vec2 ConvertFbxDataToVertexComponent(FbxVector2 const & src)
		{
			return glm::vec2(src.mData[0], src.mData[1]);
		}

		/** conversion methods : in our use cases, FbxVector4 become a vec3 (W component is unused) : used for NORMAL, BINORMAL, TANGENT */
		static glm::vec3 ConvertFbxDataToVertexComponent(FbxVector4 const & src)
		{
			return glm::vec3(src.mData[0], src.mData[1], src.mData[2]);
		}

		/** conversion methods : in our use cases, FbxColor (i.e 4 x double become a simple 4 x bytes) : used for COLOR */
		static glm::tvec4<unsigned char> ConvertFbxDataToVertexComponent(FbxColor const & src)
		{
			unsigned char r = (unsigned char)(src.mRed   * 255.0);
			unsigned char g = (unsigned char)(src.mGreen * 255.0);
			unsigned char b = (unsigned char)(src.mBlue  * 255.0);
			unsigned char a = (unsigned char)(src.mAlpha * 255.0);

			return glm::tvec4<unsigned char>(r, g, b, a);
		}

		/** access data from the element (with appropriate index), and write it the buffer */
		template<typename T>
		static void WriteFbxComponent(T const * element, SparseWriteBuffer<> & vertex_write_buffer, int control_point_index, int polygon_index, int vertex_index)
		{
			if (!IsGeometryElementValid(element))
				return;

			FbxLayerElement::EMappingMode mode = element->GetMappingMode();

			int access_index = 0;
			if (mode == FbxLayerElement::EMappingMode::eAllSame)  // address the buffer 0 ???
				access_index = 0;
			else if (mode == FbxLayerElement::EMappingMode::eByControlPoint) // address the buffer with the control point index
				access_index = control_point_index;
			else if (mode == FbxLayerElement::EMappingMode::eByPolygon) // address the buffer with the polygon index
				access_index = polygon_index;
			else if (mode == FbxLayerElement::EMappingMode::eByPolygonVertex)
				access_index = vertex_index;

			auto component = ReadFbxElement(element, access_index);
			vertex_write_buffer.Write(ConvertFbxDataToVertexComponent(component));
		}

		bool MyFbxImporter::DoImportMesh(FbxMesh * mesh)
		{
			assert(mesh != nullptr);

			// compute one vertex size
			int color_count = mesh->GetElementVertexColorCount();   // theses count represents SEMANTIC INDEX ? (maybe a vertex can have 2 color per example)
			int uv_count = mesh->GetElementUVCount();
			int normal_count = mesh->GetElementNormalCount();
			int binormal_count = mesh->GetElementBinormalCount();
			int tangent_count = mesh->GetElementTangentCount();

			//
			// STEP 1 : compute vertex declaration (ignore data per edge and none)
			//
			GPUVertexDeclaration vertex_declaration;

			vertex_declaration.Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3); // always containing a position

			for (int i = 0; i < color_count; ++i)
				UpdateVertexDeclarationWithGeometryElement(mesh->GetElementVertexColor(i), vertex_declaration, VertexAttributeSemantic::COLOR, i, VertexAttributeType::BYTE4);

			for (int i = 0; i < uv_count; ++i)
				UpdateVertexDeclarationWithGeometryElement(mesh->GetElementUV(i), vertex_declaration, VertexAttributeSemantic::TEXCOORD, i, VertexAttributeType::FLOAT2);

			for (int i = 0; i < normal_count; ++i)
				UpdateVertexDeclarationWithGeometryElement(mesh->GetElementNormal(i), vertex_declaration, VertexAttributeSemantic::NORMAL, i, VertexAttributeType::FLOAT3);

			for (int i = 0; i < binormal_count; ++i)
				UpdateVertexDeclarationWithGeometryElement(mesh->GetElementBinormal(i), vertex_declaration, VertexAttributeSemantic::BINORMAL, i, VertexAttributeType::FLOAT3);

			for (int i = 0; i < tangent_count; ++i)
				UpdateVertexDeclarationWithGeometryElement(mesh->GetElementTangent(i), vertex_declaration, VertexAttributeSemantic::TANGENT, i, VertexAttributeType::FLOAT3);

			//
			// STEP 1.1 : search if there are some skinning information
			//

			BoneTableType bone_table = GetBoneData(mesh);

			size_t max_bone_count = GetMaxBoneCount(bone_table);
			for (size_t i = 0; i < max_bone_count; ++i)
			{
				vertex_declaration.Push(VertexAttributeSemantic::BONEWEIGHT, (int)i, VertexAttributeType::FLOAT1);
				vertex_declaration.Push(VertexAttributeSemantic::BONEINDEX, (int)i, VertexAttributeType::INT1);
			}

			int vertex_byte_size = vertex_declaration.GetVertexSize();

			//
			// STEP 2 : for each polygon vertex of FBX, we generate a vertex in our own buffer
			//          Our vertex is a concatenation of :
			//            - control points position
			//            - attributes that can be shared among theses control points
			//            - attributes that can be constant inside a whole polygon
			//            - attributes that can be per vertex
			//
			//          That is far too much data, because some vertices could be shared between polygons, but this is enough for this step
			//
			// STEP 2.5 : in the same loop, we generate triangles (index buffer where all 3 consecutive index give a triangle).
			//            FBX use polygons that may have any number of vertices. We simply transforms thoses polygons into triangle fan.
			//            This is a raw approximation, but this should do the trick
			//

			// create the vertex buffer and the index buffer
			int                polygon_count = mesh->GetPolygonCount();
			int                control_count = mesh->GetControlPointsCount();
			FbxVector4 const * control_points = mesh->GetControlPoints();

			// vertex_buffer is a mixed data buffer
			SparseWriteBuffer<> vertex_write_buffer(vertex_byte_size * control_count);
			// the index buffer
			std::vector<int> index_buffer;

			// vertices that share a control point have a chance to be identical
			std::map<int, std::vector<int>> potential_equiv_vertices; // MAP[control_point_index] = {list of final vertices that may be identical}

																	  // triangulates the mesh
			int vertex_count = 0;
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				int vertex_in_polygon_count = mesh->GetPolygonSize(polygon_index);

				// XXX : create the triangulation (a triangle fan)
				//       this is not a real triangulation but it is reasonable to expect to only have 3 or 4 vertex polygons fro FBX
				for (int v = 0; v < vertex_in_polygon_count - 2; ++v)
				{
					index_buffer.push_back(vertex_count);
					index_buffer.push_back(vertex_count + v + 1);
					index_buffer.push_back(vertex_count + v + 2);
				}

				// insert the vertices
				for (int v = 0; v < vertex_in_polygon_count; ++v)
				{
					int control_point_index = mesh->GetPolygonVertex(polygon_index, v);

					// write the vertex coordinate (with a transform)
					FbxVector4 control_p = control_points[control_point_index];
					glm::vec4  point = glm::vec4(ConvertFbxDataToVertexComponent(control_p), 1.0f) * params.vertex_transforms;
					vertex_write_buffer.Write(glm::vec3(point));

					// write the color
					for (int i = 0; i < color_count; ++i)
						WriteFbxComponent(mesh->GetElementVertexColor(i), vertex_write_buffer, control_point_index, polygon_index, vertex_count);
					// write the uv
					for (int i = 0; i < uv_count; ++i)
						WriteFbxComponent(mesh->GetElementUV(i), vertex_write_buffer, control_point_index, polygon_index, vertex_count);
					// write the normal
					for (int i = 0; i < normal_count; ++i)
						WriteFbxComponent(mesh->GetElementNormal(i), vertex_write_buffer, control_point_index, polygon_index, vertex_count);
					// write the binormal
					for (int i = 0; i < binormal_count; ++i)
						WriteFbxComponent(mesh->GetElementBinormal(i), vertex_write_buffer, control_point_index, polygon_index, vertex_count);
					// write the tangent
					for (int i = 0; i < tangent_count; ++i)
						WriteFbxComponent(mesh->GetElementTangent(i), vertex_write_buffer, control_point_index, polygon_index, vertex_count);

					// write the bone information
					SkinDataType const & skin_data = bone_table[control_point_index];
					for (size_t i = 0; i < max_bone_count; ++i)
					{
						int32_t boneindex = 0;
						float   boneweight = 0.0f;
						if (i < skin_data.size()) // this will fill missing bones with 0, 0 
						{
							boneindex = skin_data[i].first;
							boneweight = skin_data[i].second;
						}
						vertex_write_buffer.Write(&boneweight, sizeof(float));
						vertex_write_buffer.Write(&boneindex, sizeof(float));
					}

					// vertex is in its potentially equivalent list
					if (params.remove_duplicated_vertex)
						potential_equiv_vertices[control_point_index].push_back(vertex_count);

					++vertex_count;
				}
			}

			//
			// STEP 3 : optimization.
			//          We have generated far two much vertices. It's time to remove some duplication.
			//          2 vertices can only be identical if they use the same control point.
			//          So we use a map
			//
			//                MAP[control_point_index] = {list of final vertices that COULD be identical} 
			//
			//          This helps reducing the number of comparaison
			//
			//          Then we compare pairs of vertices and store results in an equivalent table.
			// 

			// create a contigus buffer for the vertices
			Buffer<char> vertices = SharedBufferPolicy<char>::NewBuffer(vertex_write_buffer.GetWrittenSize());
			vertex_write_buffer.CopyToBuffer(vertices.data, vertices.bufsize);

			// try to optimize the mesh
			int removed_vertex_count = 0;
			if (params.remove_duplicated_vertex)
			{
#if _DEBUG
				size_t optimized_comparaison_count = 0;
				for (auto & it = potential_equiv_vertices.begin(); it != potential_equiv_vertices.end(); ++it)
					optimized_comparaison_count += it->second.size() * (it->second.size() - 1);
				size_t forcebrute_comparaison_count = (size_t)vertex_count * (vertex_count - 1);
				//assert(optimized_comparaison_count <= forcebrute_comparaison_count);

				size_t effective_comparaison_count = 0;
#endif

				// prepare an equivalence table
				std::vector<int> equiv_classes;
				equiv_classes.reserve(vertex_count);
				for (int i = 0; i < vertex_count; ++i)
					equiv_classes.push_back(i);  // one equivalence class per vertex

												 // compare vertices by pairs and compute the equivalence table
				for (auto & it = potential_equiv_vertices.begin(); it != potential_equiv_vertices.end(); ++it)
				{
					size_t count = it->second.size();
					for (size_t i = 0; i < count - 1; ++i)
					{
						int v1 = it->second[i];

						if (equiv_classes[v1] != v1) // if current V1 is not in its own class, so is for other equivalent vertices next to him
							continue;

						for (size_t j = i + 1; j < count; ++j)
						{
#if _DEBUG
							++effective_comparaison_count;
#endif
							int v2 = it->second[j];
							if (memcmp(&vertices.data[v1 * vertex_byte_size], &vertices.data[v2 * vertex_byte_size], vertex_byte_size) == 0) // compare 2 vertices
								equiv_classes[v2] = v1;
						}
					}
				}
#if _DEBUG
				assert(effective_comparaison_count <= optimized_comparaison_count);
#endif

				//
				// STEP 3.5 : optimization.
				//
				//            Using our equivalent table, we can say that
				//
				//              -vertex that are in another equivalence class are duplication and so can be removed
				//              -whenever a vertex is removed this create an empty place for all next vertices
				//              -vertex that are still in their equivalence class remains unchanged
				//               but they can still be displaced to create a more compact vertex buffer
				//
				// ex :
				//
				//  index           |  0   1   2   3   4   5   6   7   8   9
				//  --------------------------------------------------------
				//  equiv class     |  0   1   2   1   4   5   2   5   8   9 
				//                                ---         --- ---
				//                                 |           |   |
				//                                   can be removed
				//
				//
				// after optimization, this gives
				//
				//  index           |  0   1   2   3   4   5   6   7   8   9
				//  --------------------------------------------------------
				//  equiv class     |  0   1   2   1   3   4   2   4   5   6 
				//                                    --- --- --- --- --- ---
				//                                     |   |   |   |   |   |
				//                               must be displaced to compact buffer
				//                                      

				// apply the equivalence table in vertex buffer
				// each vertex that are not in than their own equiv class make a free row for next vertices in the array

				for (int i = 0; i < vertex_count; ++i)
				{
					if (equiv_classes[i] == i) // this vertex may be displaced in free row
					{
						if (removed_vertex_count > 0)
						{
							equiv_classes[i] = equiv_classes[i] - removed_vertex_count;
							memcpy(&vertices.data[equiv_classes[i] * vertex_byte_size], &vertices.data[i * vertex_byte_size], vertex_byte_size);
						}
					}
					else
					{
						equiv_classes[i] = equiv_classes[equiv_classes[i]]; // B is equivalent to A, but maybe A has been displaced. Take care of that
						++removed_vertex_count; // this vertex is equivalent to a previous one. This make row for another vertex to come
					}
				}
				// apply the equivalence table in index buffer
				size_t index_count = index_buffer.size();
				for (size_t i = 0; i < index_count; ++i)
					index_buffer[i] = equiv_classes[index_buffer[i]];


#if 0
				//
				// STEP 4 : spacial locality of vertices. 
				//          try to avoid large distances between vertex indices 
				//          [1, 2, 3] is better than [2, 50, 100]
				//

				std::vector<int> tmp = index_buffer;

				std::vector<int> spacial_optimized_index_buffer;
				spacial_optimized_index_buffer.reserve(index_count);
				spacial_optimized_index_buffer.insert(spacial_optimized_index_buffer.begin(), index_count, -1);

				int next_free_index = 0;
				for (size_t i = 0; i < index_count; ++i)
				{
					int idx = index_buffer[i];

					int optimized_index = spacial_optimized_index_buffer[idx];
					if (optimized_index < 0)
					{
						spacial_optimized_index_buffer[idx] = optimized_index = next_free_index++;


						// displace vertex
					}
					index_buffer[i] = optimized_index;
				}

				for (size_t i = 0; i < index_count; ++i)
					if (tmp[i] != index_buffer[i])
						i = i;

				index_count = index_count;

#endif
			}

			Buffer<char> optimize_vertices;
			optimize_vertices.data = vertices.data;
			optimize_vertices.bufsize = vertices.bufsize - removed_vertex_count * vertex_byte_size;

			if (optimize_vertices.bufsize > 0 && index_buffer.size())
			{
				GPUSimpleMesh * m = DoCreateMesh(vertex_declaration, optimize_vertices, index_buffer);
				if (m != nullptr)
				{
					output->meshes.push_back(m);
					char const * name = mesh->GetName();
					name = name;
				}
			}
			return true;
		}

		GPUSimpleMesh * MyFbxImporter::DoCreateMesh(GPUVertexDeclaration const & vertex_declaration, Buffer<char> vertices, std::vector<int> const & index_buffer)
		{
			GPUSimpleMesh * result = new GPUSimpleMesh;
			if (result != nullptr)
			{
				GLuint va = 0;
				GLuint vb = 0;
				GLuint ib = 0;

				// shuxxx
				assert(0);
#if 0
				if (GLTools::GenerateVertexAndIndexBuffers(&va, &vb, &ib, false, false)) // vertex_buffer and index_buffer are static
				{
					result->vertex_declaration = vertex_declaration;

					GPUDrawPrimitive primitive;
					primitive.count = (int)index_buffer.size();
					primitive.indexed = true;
					primitive.primitive_type = GL_TRIANGLES;
					primitive.start = 0;
					primitive.base_vertex_index = 0;
					result->primitives.push_back(primitive);

					//result->vertex_array  = new GPUVertexArray(va);
					result->vertex_buffer = new GPUBuffer(vb, false); // static
					result->index_buffer = new GPUBuffer(ib, false);

					// fill the buffers
					glNamedBufferData(vb, vertices.bufsize, vertices.data, GL_STATIC_DRAW);
					glNamedBufferData(ib, index_buffer.size() * sizeof(index_buffer[0]), &index_buffer[0], GL_STATIC_DRAW);

					// Initialize the vertex arrays
					glVertexArrayElementBuffer(va, ib);

					glBindVertexArray(va); // shuxxx glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, vb);

					return result;
				}
#endif
				delete(result);
			}
			return nullptr;
		}

}; // namespace chaos
