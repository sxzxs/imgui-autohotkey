Persistent
sendkey(key)
{
    SendInput(key)
}

sound_play(sound)
{
    SoundPlay(sound)
}

highlight_position(x, y)
{
	HighlightOutline(x - 5, y - 5, x + 5, y + 5,, "328ad6", 255, 1000)
}

;HighlightOutline(200, 0, 400, 200,,"328ad6", 255, 3000)
class HighlightOutline 
{
    gui := []
	__New(x1, y1, x2, y2, b := 3,color := "red", Transparent := 255, time_out := unset) 
    {
        this.gui.Length := 4
		Loop 4 {
            this.gui[A_index] := Gui("-Caption +AlwaysOnTop +ToolWindow -DPIScale +E0x20 +E0x00080000")
            this.gui[A_index].BackColor := color
			DllCall("SetLayeredWindowAttributes", "Ptr", this.gui[A_index].hwnd, "Uint", 0, "Uchar", Transparent, "int", 2)
		}
        if(IsSet(time_out))
        {
            this.timer := ObjBindMethod(this, "Destroy")
            this.Show(x1, y1, x2, y2)
            SetTimer(this.timer, -time_out)
        }
	}
	Show(x1, y1, x2, y2, b := 3) 
    {
		Try 
        {
            this.gui[1].Show("NA x" x1-b " y" y1-b " w" x2-x1+b*2 " h" b)
            this.gui[2].show("NA x" x2 " y" y1 " w" b " h" y2-y1)
            this.gui[3].Show("NA x" x1-b " y" y2 " w" x2-x1+2*b " h" b)
            this.gui[4].Show("NA x" x1-b " y" y1 " w" b " h" y2-y1)
		}
	}
	Hide() 
    {
		Loop(4)
        {
            try 
            { 
                this.gui[A_Index].Hide()
            }
        }
	}
	Destroy()
    {
        this.timer := 0
		Loop(4)
        {
            try
            {
                this.gui[A_Index].Destroy()
            }
        }
	}
}