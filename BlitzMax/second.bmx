
Graphics 800, 600, 0

Local Grid$[3,3]

Grid[0,0] = "toto"

Local OtherArray[][] = [[1,1,1], [2, 2]]


Local arr0[]= [0, 1, 2, 3, 4, 5]

arr0 = arr0[4..]
arr0.sort()

'=====================================================================

Type Spaceship

	Field Name$ = "toto"
	
	Global ListOfSpaceship:TList
	
	Method ChangeName()
	
		Name :+ Name
			
	End Method
	
	Function CreateSpaceship:Spaceship()
	
		Local result:spaceship = New Spaceship
		
		
		If Not ListOfSpaceship Then		
			ListOfSpaceship = CreateList()		
		End If
		
		ListAddLast(ListOfSpaceship, result)
		
		Return result
	
	End Function

End Type

Type Spaceship2

	Field Name$ = "toto"

End Type

'=====================================================================



Local player1:Spaceship = Spaceship.CreateSpaceship()
Local player2:Spaceship = player1


Print "tutu : " + Spaceship.ListOfSpaceship.Count()

Local p1:Spaceship = Spaceship(Spaceship.ListOfSpaceship.First())
p1.Name = "Gwen"

'=====================================================================

' Local p:Object   y a t'il de l heritage ??

'player1.Name = "titi"
player1.ChangeName()
player1.ChangeName()
player1.ChangeName()
Print player2.Name 

Type PPP

End Type


Repeat
Until True

Type MMM 

	Field value = 3


End Type

Type MyType Extends Object


Field value = 3
	Method toto()
	
	Self.value = 666
	
	Print("hello world")
	
	End Method


End Type


New MyType.toto()




Local AAA=3

Select AAA

	Case 3
		Print "AAA = 3"


End Select



Local L:TList = CreateList()
ListAddLast(L, New Spaceship2)
ListAddLast(L, player1)
ListAddLast(L, New Spaceship2)
ListAddLast(L, player1)


For Local o:Spaceship = EachIn L 'skip elements of not the good type (should have 4 output, but only 2 diplayed)

	Print "xx : " +o.name
	Print "pu"
Next



Local c = L.count()
Print "count = " + c


For Local i = EachIn arr0
	Print i
Next

Print "Len = " + arr0.length


While Not KeyDown(Key_Escape)


  Flip; Cls

Wend
