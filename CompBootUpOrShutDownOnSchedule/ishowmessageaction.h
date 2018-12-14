#ifndef __IShowMessageAction_INTERFACE_DEFINED__
#define __IShowMessageAction_INTERFACE_DEFINED__


#include "taskservice.h"

// Интерфейс представляющий собой действие, которое показывает окно сообщения.
// Этот интерфейс больше не поддерживается.
// Вместо него предлагается использовать Windows-скрипты и функцию MsgBox.

//DEFINE_GUID(IID_IShowMessageAction, 0x505E9E68, 0xAF89, 0x46B8, 0xA3,0x0F, 0x56,0x16,0x2A,0x83,0xD5,0x37);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("505E9E68-AF89-46B8-A30F-56162A83D537")
IShowMessageAction : public IAction
{
    // Получить заголовок сообщения:
    virtual HRESULT get_Title(BSTR *pTitle) = 0;

    // Установить заголовок сообщения:
    virtual HRESULT put_Title(BSTR title) = 0;

    // Получить текст сообщения:
    virtual HRESULT get_MessageBody(BSTR *pMessageBody) = 0;

    // Установить текст сообщения:
    virtual HRESULT put_MessageBody(BSTR messageBody) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IShowMessageAction, 0x505E9E68, 0xAF89, 0x46B8, 0xA3,0x0F, 0x56,0x16,0x2A,0x83,0xD5,0x37)
#endif
#else
#endif
#endif

