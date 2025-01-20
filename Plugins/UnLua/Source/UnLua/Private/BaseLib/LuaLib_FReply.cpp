
#include "LuaCore.h"
#include "UnLuaEx.h"
#include "Components/SlateWrapperTypes.h"

static int32 FEventReply_New(lua_State *L)
{
    int32 NumParams = lua_gettop(L);
    bool bHandle = true;
    if(NumParams > 1)
        bHandle = (bool)lua_toboolean(L,2);
    void *Userdata = NewTypedUserdata(L, FEventReply);
    FEventReply *V = new(Userdata) FEventReply(bHandle );
    
    return 1;
}


static const luaL_Reg FEventReplyLib[] =
{
    { "__call", FEventReply_New },
    { nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(FEventReply)
    ADD_LIB(FEventReplyLib)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(FEventReply)
