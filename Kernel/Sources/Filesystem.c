//
//  Filesystem.c
//  Apollo
//
//  Created by Dietmar Planitzer on 11/07/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#include "Filesystem.h"
#include "FilesystemManager.h"

////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Inode
////////////////////////////////////////////////////////////////////////////////

ErrorCode Inode_AbstractCreate(ClassRef pClass, Int8 type, InodeId id, FilesystemId fsid, FilePermissions permissions, User user, InodeRef _Nullable * _Nonnull pOutNode)
{
    decl_try_err();
    InodeRef pNode;

    try(_Object_Create(pClass, 0, (ObjectRef*)&pNode));
    pNode->type = type;
    pNode->permissions = permissions;
    pNode->user = user;
    pNode->noid = id;
    pNode->fsid = fsid;

    *pOutNode = pNode;
    return EOK;

catch:
    *pOutNode = NULL;
    return err;
}

// Returns a strong reference to the filesystem that owns the given note. Returns
// NULL if the filesystem isn't mounted.
FilesystemRef Inode_CopyFilesystem(InodeRef _Nonnull self)
{
    return FilesystemManager_CopyFilesystemForId(gFilesystemManager, Inode_GetFilesystemId(self));
}

// Returns EOK if the given user has at least the permissions 'permission' to
// access and/or manipulate the node; a suitable error code otherwise. The
// 'permission' parameter represents a set of the permissions of a single
// permission scope.
ErrorCode Inode_CheckAccess(InodeRef _Nonnull self, User user, FilePermissions permission)
{
    // XXX revisit this once we put a proper user permission model in place
    // XXX forcing superuser off for now since it's the only user we got at this
    // XXX time and we want the file permissions to actually do something.
    //if (user.uid == kRootUserId) {
    //    return EOK;
    //}

    FilePermissions reqPerms = 0;

    if (Inode_GetUserId(self) == user.uid) {
        reqPerms = FilePermissions_Make(0, 0, permission);
    }
    else if (Inode_GetGroupId(self) == user.gid) {
        reqPerms = FilePermissions_Make(0, permission, 0);
    }
    else {
        reqPerms = FilePermissions_Make(permission, 0, 0);
    }

    if ((Inode_GetFilePermissions(self) & reqPerms) == reqPerms) {
        return EOK;
    }

    return EACCESS;
}

// Returns true if the receiver and 'pOther' are the same node; false otherwise
Bool Inode_Equals(InodeRef _Nonnull self, InodeRef _Nonnull pOther)
{
    return self->fsid == pOther->fsid && self->noid == pOther->noid;
}

void Inode_deinit(InodeRef _Nonnull self)
{
}

CLASS_METHODS(Inode, Object,
OVERRIDE_METHOD_IMPL(deinit, Inode, Object)
);


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Filesystem
////////////////////////////////////////////////////////////////////////////////

// Returns the next available FSID.
static FilesystemId Filesystem_GetNextAvailableId(void)
{
    // XXX want to:
    // XXX handle overflow (wrap around)
    // XXX make sure the generated id isn't actually in use by someone else
    static volatile AtomicInt gNextAvailableId = 0;
    return (FilesystemId) AtomicInt_Increment(&gNextAvailableId);
}

// Creates an instance of a filesystem subclass. Users of a concrete filesystem
// should not use this function to allocate an instance of the concrete filesystem.
// This function is for use by Filesystem subclassers to define the filesystem
// specific instance allocation function.
ErrorCode Filesystem_Create(ClassRef pClass, FilesystemRef _Nullable * _Nonnull pOutFileSys)
{
    decl_try_err();
    FilesystemRef pFileSys;

    try(_Object_Create(pClass, 0, (ObjectRef*)&pFileSys));
    pFileSys->fsid = Filesystem_GetNextAvailableId();
    *pOutFileSys = pFileSys;
    return EOK;

catch:
    *pOutFileSys = NULL;
    return err;
}

// Invoked when an instance of this file system is mounted.
ErrorCode Filesystem_onMount(void* _Nonnull self, const Byte* _Nonnull pParams, ByteCount paramsSize)
{
    return EOK;
}

// Invoked when a mounted instance of this file system is unmounted. A file
// system may return an error. Note however that this error is purely advisory
// and the file system implementation is required to do everything it can to
// successfully unmount. Unmount errors are ignored and the file system manager
// will complete the unmount in any case.
ErrorCode Filesystem_onUnmount(void* _Nonnull self)
{
    return EOK;
}


// Returns EOK and the parent node of the given node if it exists and ENOENT
// and NULL if the given node is the root node of the namespace. 
InodeRef _Nonnull Filesystem_copyRootNode(FilesystemRef _Nonnull self)
{
    abort();
    // NOT REACHED
    return NULL;
}

// Returns EOK and the parent node of the given node if it exists and ENOENT
// and NULL if the given node is the root node of the namespace. 
ErrorCode Filesystem_copyParentOfNode(FilesystemRef _Nonnull self, InodeRef _Nonnull pNode, User user, InodeRef _Nullable * _Nonnull pOutNode)
{
    *pOutNode = NULL;
    return ENOENT;
}

// Returns EOK and the node that corresponds to the tuple (parent-node, name),
// if that node exists. Otherwise returns ENOENT and NULL.  Note that this
// function will always only be called with proper node names. Eg never with
// "." nor "..".
ErrorCode Filesystem_copyNodeForName(FilesystemRef _Nonnull self, InodeRef _Nonnull pParentNode, const PathComponent* pComponent, User user, InodeRef _Nullable * _Nonnull pOutNode)
{
    *pOutNode = NULL;
    return ENOENT;
}

ErrorCode Filesystem_getNameOfNode(FilesystemRef _Nonnull self, InodeRef _Nonnull pParentNode, InodeRef _Nonnull pNode, User user, MutablePathComponent* _Nonnull pComponent)
{
    pComponent->count = 0;
    return ENOENT;
}

// If the node is a directory and another file system is mounted at this directory,
// then this function returns the filesystem ID of the mounted directory; otherwise
// 0 is returned.
FilesystemId Filesystem_getFilesystemMountedOnNode(FilesystemRef _Nonnull self, InodeRef _Nonnull pNode)
{
    return 0;
}

// Marks the given directory node as a mount point at which the filesystem
// with the given filesystem ID is mounted. Converts the node back into a
// regular directory node if the give filesystem ID is 0.
void Filesystem_setFilesystemMountedOnNode(FilesystemRef _Nonnull self, InodeRef _Nonnull pNode, FilesystemId fsid)
{
}

CLASS_METHODS(Filesystem, IOResource,
METHOD_IMPL(onMount, Filesystem)
METHOD_IMPL(onUnmount, Filesystem)
METHOD_IMPL(copyRootNode, Filesystem)
METHOD_IMPL(copyParentOfNode, Filesystem)
METHOD_IMPL(copyNodeForName, Filesystem)
METHOD_IMPL(getNameOfNode, Filesystem)
METHOD_IMPL(getFilesystemMountedOnNode, Filesystem)
METHOD_IMPL(setFilesystemMountedOnNode, Filesystem)
);
