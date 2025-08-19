// Some changes by Michael Zeilfelder
// Not sure which one again (gotta diff some day), but at least 32-bit support

#include "CBatchingMesh.h"
#include "CDynamicMeshBuffer.h"

namespace irr
{
namespace scene
{

CBatchingMesh::CBatchingMesh() 
 : Box(core::vector3df(0,0,0)), IsDirty(false), IsFinal(false)
{
}

CBatchingMesh::~CBatchingMesh()
{
	for (u32 i=0; i < DestBuffers.size(); ++i)
		DestBuffers[i].Buffer->drop();

	for (u32 i=0; i < SourceBuffers.size(); ++i)
		SourceBuffers[i]->drop();
}

bool CBatchingMesh::isDirty(s32 id)
{
	if ((u32)id > DestBuffers.size())
		return IsDirty;
	else
		return DestBuffers[id].IsDirty;
}

//! refreshes the internal buffers from source
void CBatchingMesh::update()
{
	// allocate the index and vertex arrays
	u32 i;
	for (i=0; i<DestBuffers.size(); ++i)
	{
		if (DestBuffers[i].IndexCount != DestBuffers[i].Buffer->getIndexCount() || 
			DestBuffers[i].VertexCount != DestBuffers[i].Buffer->getVertexCount())
		{
			DestBuffers[i].IsDirty = true;

			CDynamicMeshBuffer* mb = (CDynamicMeshBuffer*)DestBuffers[i].Buffer;
			mb->getVertexBuffer().set_used(DestBuffers[i].VertexCount);
			mb->getIndexBuffer().set_used(DestBuffers[i].IndexCount);
		}
	}

	// refresh dirty buffers from source
	for (i=0; i<BufferReferences.size(); ++i)
	{
		if (DestBuffers[BufferReferences[i].DestReference].IsDirty)
		{
			updateDestFromSourceBuffer(i);
		}
	}

	// calculate bounding boxes
	for (i=0; i< DestBuffers.size(); ++i)
	{
		if (DestBuffers[i].IsDirty)
		{
			DestBuffers[i].Buffer->recalculateBoundingBox();
			// reset dirty state too
			DestBuffers[i].IsDirty = false;
		}
	}

	IsDirty = false;
	recalculateBoundingBox();
}

//! adds a mesh to the buffers with the given offset
/** \Returns Returns an array of ID numbers */
core::array<s32> CBatchingMesh::addMesh(const IMesh* mesh, const core::array<video::SMaterial>& materials, core::vector3df pos, core::vector3df rot, core::vector3df scale)
{
	core::matrix4 m;
	m.setRotationDegrees(rot);
	m.setTranslation(pos);

	core::matrix4 scalem;
	scalem.setScale(scale);
	m *= scalem;

	return addMesh(mesh, materials, m);
}

//! adds a mesh with the given transformation
core::array<s32> CBatchingMesh::addMesh(const IMesh* mesh, const core::array<video::SMaterial>& materials, const core::matrix4 &transform)
{
	core::array<s32> bufferNos;

	if (!mesh)
		return bufferNos;

	u32 i;
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		bufferNos.push_back(addMeshBuffer(mesh->getMeshBuffer(i), transform, materials.empty() ? nullptr : &(materials[i])));
	}

	return bufferNos;
}

//! adds a mesh buffer with the given transformation
/** \Return Returns the ID of this mesh buffer */
s32 CBatchingMesh::addMeshBuffer(const IMeshBuffer* buffer, core::vector3df pos, core::vector3df rot, core::vector3df scale, const video::SMaterial * material)
{
	core::matrix4 m;
	m.setRotationDegrees(rot);
	m.setTranslation(pos);

	core::matrix4 scalem;
	scalem.setScale(scale);
	m *= scalem;

	return addMeshBuffer(buffer, m, material);
}

//! adds a mesh with the given transformation
/** \Return Returns the ID of this mesh buffer */
s32 CBatchingMesh::addMeshBuffer(const IMeshBuffer* buffer, const core::matrix4 &transform, const video::SMaterial * material)
{
	if (!buffer || IsFinal)
		return -1;

	u32 i=0;
	const video::SMaterial& m = material ? *material : buffer->getMaterial();

	// find destination buffer to use. Needs same material and vertex type
	bool found=false;
	video::E_VERTEX_TYPE vt = buffer->getVertexType();
	for (i=0; i<DestBuffers.size(); ++i)
	{
		if (DestBuffers[i].Buffer->getVertexType() == vt && 
			DestBuffers[i].Buffer->getMaterial() == m)
		{
			found = true;
			DestBuffers[i].IndexCount += buffer->getIndexCount();
			DestBuffers[i].VertexCount += buffer->getVertexCount();
			break;
		}
	}

	if (!found)
	{
		// we need a new destination buffer
		SDestBufferReference db;
		db.Buffer = new CDynamicMeshBuffer(vt, irr::video::EIT_32BIT);
		db.IndexCount = buffer->getIndexCount();
		db.VertexCount = buffer->getVertexCount();
		db.Buffer->getMaterial() = m;
		db.IsDirty = true;

		DestBuffers.push_back(db);
	}
	// now we add the mesh reference
	SBufferReference r;
	r.DestReference = i;
	r.SourceBuffer = buffer;
	r.Transform = transform;
	r.IndexCount = buffer->getIndexCount();
	r.VertexCount = buffer->getVertexCount();
	r.FirstIndex = DestBuffers[i].IndexCount - r.IndexCount;
	r.FirstVertex = DestBuffers[i].VertexCount - r.VertexCount; 
	r.Initialized = false;
	BufferReferences.push_back(r);
	addSourceBuffer(buffer);

	IsDirty = true;
	return BufferReferences.size()-1;
}

//! updates bounding box from internal buffers
void CBatchingMesh::recalculateBoundingBox()
{
	if (DestBuffers.empty())
	{
		Box.reset(0,0,0);
	}
	else
	{
		Box = DestBuffers[0].Buffer->getBoundingBox();
		for (u32 i=1; i < DestBuffers.size(); ++i)
			Box.addInternalBox(DestBuffers[i].Buffer->getBoundingBox());
	}
}


/* Standard IMesh functions */

//! Returns the amount of mesh buffers.
/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
u32 CBatchingMesh::getMeshBufferCount() const
{
	return DestBuffers.size();
}

//! Returns pointer to a mesh buffer.
/** \param nr: Zero based index of the mesh buffer. The maximum value is
getMeshBufferCount() - 1;
\return Returns the pointer to the mesh buffer or 
nullptr if there is no such mesh buffer. */
IMeshBuffer* CBatchingMesh::getMeshBuffer(u32 nr) const
{
	if (nr < DestBuffers.size())
		return DestBuffers[nr].Buffer;
	else
		return nullptr;
}

//! Returns pointer to a mesh buffer which fits a material
IMeshBuffer* CBatchingMesh::getMeshBuffer( const video::SMaterial &material) const 
{ 
	for ( u32 i=0; i<DestBuffers.size(); ++i )
		if ( DestBuffers[i].Buffer->getMaterial() == material )
			return DestBuffers[i].Buffer;
	return nullptr;
}

//! Returns an axis aligned bounding box of the mesh.
/** \return A bounding box of this mesh is returned. */
const core::aabbox3d<f32>& CBatchingMesh::getBoundingBox() const
{
	return Box;
}

//! set user axis aligned bounding box
void CBatchingMesh::setBoundingBox( const core::aabbox3df& box)
{
	Box = box;
}

//! Sets a flag of all contained materials to a new value.
/** \param flag: Flag to set in all materials.
 \param newvalue: New value to set in all materials. */
void CBatchingMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (u32 i=0; i<DestBuffers.size(); ++i)
		DestBuffers[i].Buffer->getMaterial().setFlag(flag, newvalue);
}

//! drops all buffers and clears internal states
void CBatchingMesh::clear()
{
	u32 i;
	for (i=0; i < DestBuffers.size(); ++i)
		DestBuffers[i].Buffer->drop();

	for (i=0; i < SourceBuffers.size(); ++i)
		SourceBuffers[i]->drop();

	BufferReferences.clear();
	DestBuffers.clear();
	SourceBuffers.clear();

	IsDirty = false;
	IsFinal = false;
}

//! first updates the mesh, then drops all source buffers.
/** once this mesh has been finalized, it cannot be changed again! */
void CBatchingMesh::finalize()
{
	update();

	for (u32 i=0; i < SourceBuffers.size(); ++i)
		SourceBuffers[i]->drop();

	SourceBuffers.clear();

	IsFinal = true;
}

//! Moves a mesh
core::array<bool> CBatchingMesh::moveMesh(const core::array<s32>& bufferIDs, const core::matrix4 &newMatrix)
{
	core::array<bool> result;
	result.reallocate(bufferIDs.size());
	for (u32 i=0; i<bufferIDs.size(); ++i)
		result.push_back(moveMeshBuffer(bufferIDs[i], newMatrix));

	return result;
}


//! Moves a mesh buffer
bool CBatchingMesh::moveMeshBuffer(const s32 id, const core::matrix4 &newMatrix)
{
	if ((u32)id > BufferReferences.size() || IsFinal )
		return false;

	BufferReferences[id].Transform = newMatrix;

	// is the source buffer dirty?
	if (!DestBuffers[BufferReferences[id].DestReference].IsDirty)
	{
		// transform each vertex and normal
		updateDestFromSourceBuffer(id);
		DestBuffers[BufferReferences[id].DestReference].Buffer->recalculateBoundingBox();
	}
	return true;
}


//! returns the source buffer, if available
const IMeshBuffer* CBatchingMesh::getSourceBuffer(s32 id) const
{
	if ((u32)id > BufferReferences.size() || IsFinal)
		return 0;
	else
		return BufferReferences[id].SourceBuffer;
}

//! returns the matrix of the source buffer
core::matrix4 CBatchingMesh::getSourceBufferMatrix(s32 id) const
{
	core::matrix4 ret;
	if ((u32)id > BufferReferences.size() || IsFinal)
		ret.makeIdentity();
	else
		ret = BufferReferences[id].Transform;

	return ret;
}


//! returns the number of source buffers
u32 CBatchingMesh::getSourceBufferCount() const
{
	return BufferReferences.size();
}

// private functions

void CBatchingMesh::updateDestFromSourceBuffer(u32 i)
{
	u32 fi = BufferReferences[i].FirstIndex;
	u32 fv = BufferReferences[i].FirstVertex;
	u32 ic = BufferReferences[i].IndexCount;
	u32 vc = BufferReferences[i].VertexCount;
	CDynamicMeshBuffer* destMeshBuffer = (CDynamicMeshBuffer*) DestBuffers[BufferReferences[i].DestReference].Buffer;

	IIndexBuffer& destIndexBuffer = destMeshBuffer->getIndexBuffer();
	if ( BufferReferences[i].SourceBuffer->getIndexType() == video::EIT_16BIT )
	{
		const irr::u16* indices = BufferReferences[i].SourceBuffer->getIndices();
		for (u32 x=0; x < ic; ++x)
			destIndexBuffer.setValue(x+fi, indices[x]+fv);
	}
	else if ( BufferReferences[i].SourceBuffer->getIndexType() == video::EIT_32BIT )
	{
		const irr::u32* indices = (const irr::u32*)BufferReferences[i].SourceBuffer->getIndices();
		for (u32 x=0; x < ic; ++x)
			destIndexBuffer.setValue(x+fi, indices[x]+fv);
	}

	// Note that we check earlier that vertex types are identical (important as this would seriously mess up otherwise)
	const video::E_VERTEX_TYPE vt = BufferReferences[i].SourceBuffer->getVertexType();
	const void* sourceVertices = BufferReferences[i].SourceBuffer->getVertices();
	IVertexBuffer& destVertexBuffer = destMeshBuffer->getVertexBuffer();
	const core::matrix4& m = BufferReferences[i].Transform;
	core::matrix4 matNormals(m);
	if ( matNormals.makeInverse() )
		matNormals = matNormals.getTransposed();

	// Check if normals need to be normalized again
	irr::core::vector3df scaleAbs = matNormals.getScale();
	scaleAbs.set(fabsf(scaleAbs.X), fabsf(scaleAbs.Y), fabsf(scaleAbs.Z));
	bool needNormalizing = !scaleAbs.equals(core::vector3df(1.f, 1.f, 1.f));

	for (u32 x=0; x < vc; ++x)
	{
		u32 x2 = x+fv;
		switch ( vt )
		{
			case video::EVT_STANDARD:
				destVertexBuffer[x2] = static_cast<const video::S3DVertex*>(sourceVertices)[x];
				break;
			case video::EVT_2TCOORDS:
				static_cast<video::S3DVertex2TCoords&>(destVertexBuffer[x2]) = static_cast<const video::S3DVertex2TCoords*>(sourceVertices)[x];
				break;
			case video::EVT_TANGENTS:
				static_cast<video::S3DVertexTangents&>(destVertexBuffer[x2]) = static_cast<const video::S3DVertexTangents*>(sourceVertices)[x];
				break;
		}
		m.transformVect(destVertexBuffer[x2].Pos);

		matNormals.rotateVect(destVertexBuffer[x2].Normal);	// what I think it should be... but needs some testing
		if ( needNormalizing )
			destVertexBuffer[x2].Normal.normalize();	// matrix can have scaling which gets applied
	}
}

void CBatchingMesh::addSourceBuffer(const IMeshBuffer *source)
{
	if ( SourceBuffers.linear_search(source) < 0 )
	{
		source->grab();
		SourceBuffers.push_back(source);
	}
}

void CBatchingMesh::setHardwareMappingHint(E_HARDWARE_MAPPING mapping, E_BUFFER_TYPE type)
{
	for (u32 i=0; i < DestBuffers.size(); ++i)
		DestBuffers[i].Buffer->setHardwareMappingHint(mapping, type);
}


void CBatchingMesh::setDirty(E_BUFFER_TYPE type)
{
	for (u32 i=0; i < DestBuffers.size(); ++i)
		DestBuffers[i].Buffer->setDirty(type);
}

} // namespace scene
} // namespace irr

