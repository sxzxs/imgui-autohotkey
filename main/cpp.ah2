Persistent
hotkey("^6", call_cpp)
HotIfWinActive "ahk_class RiotWindowClass"
hotkey("e", cutting.Bind('yellow'))
hotkey("w", cutting.Bind('blue'))
hotkey("2", cutting.Bind('red'))
public_data := 9.9999
call_cpp(*)
{
	DllCall(Integer(A_Args[1]))
}

cutting(card, key)
{
	DllCall(Integer(A_Args[2]), "str", card)
}

set_public_data(data := "")
{
	global public_data
}

get_public_data()
{
	return public_data
}