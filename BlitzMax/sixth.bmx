Graphics 800, 600

SetClsColor 255, 0, 0

'============================================================

Type Player

	Field position:SVec2F = New SVec2F(0.0, 0.0)
	Field index = 0
	
	Method New(index = 0)
	
		Self.index = index	
		Players.AddLast(Self)
	
	End Method
	
	Method Update()
	
		Local jx# = JoyX(index)
		Local jy# = JoyY(index)
		
		Local DeathZone# = 0.3
		
		If (jx < DeathZone And jx > -DeathZone) Then
			jx = 0
		End If
		If (jy < DeathZone And jy > -DeathZone) Then
			jy = 0
		End If		
		
		Local speed# = 5.0
		
		position = New SVec2F(position.x + jx * speed, position.y + jy * speed)
		
		'position[0] :+ (jx * speed)
		'position[1] :+ (jy * speed)
		
	
	End Method
	
	Method Draw()
	
		SetColor 255, 255, 255
		
		Local size# = 40
		DrawRect position.x - size / 2, position.y - size / 2, size, size
	
	End Method
	
	Global Players:TList = New TList

End Type

New Player(0)
New Player(1)



'============================================================


While Not KeyDown(Key_Escape) And Not AppTerminate()

	'rendering
	Cls
	For Local p:Player = EachIn(Player.Players)
	  p.Draw()
	Next
	
	Flip
	'updating
	For Local p:Player = EachIn(Player.Players)
	  p.Update()
	Next
	

Wend



Print "Hello"