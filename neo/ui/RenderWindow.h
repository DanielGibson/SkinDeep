/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __RENDERWINDOW_H
#define __RENDERWINDOW_H

#include "renderer/RenderWorld.h"
#include "ui/Window.h"

class idUserInterfaceLocal;
class idMD5Anim;

class idRenderWindow : public idWindow {
public:
	idRenderWindow(idUserInterfaceLocal *gui);
	idRenderWindow(idDeviceContext *d, idUserInterfaceLocal *gui);
	virtual ~idRenderWindow();
	
	virtual void WriteToSaveGame( idSaveGame *savefile ) const; // blendo eric: savegame pass 1
	virtual void ReadFromSaveGame( idRestoreGame *savefile );

	virtual void PostParse();
	virtual void Draw(int time, float x, float y);
	virtual size_t Allocated(){return idWindow::Allocated();};
//
//
	virtual idWinVar *GetWinVarByName(const char *_name, bool winLookup = false, drawWin_t** owner = NULL);
//

private:
	void CommonInit();
	virtual bool ParseInternalVar(const char *name, idParser *src);
	void Render(int time);
	void PreRender();
	void BuildAnimation(int time);
	renderView_t refdef;
	idRenderWorld *world;
	renderEntity_t worldEntity;
	renderLight_t rLight;
	const idMD5Anim *modelAnim;

	qhandle_t	worldModelDef;
	qhandle_t	lightDef;
	qhandle_t   modelDef;
	idWinStr modelName;
	idStr lastModelName;
	idWinStr animName;
	idStr	 animClass;
	idWinVec4 lightOrigin;
	idWinVec4 lightColor;
	idWinVec4 modelOrigin;
	idWinVec4 modelRotate;
	idWinVec4 viewOffset;
	idWinBool needsRender;
	int animLength;
	int animEndTime;
	bool updateAnimation;

	//BC gui model skin
	idWinStr	skinName;
	idWinFloat	modelFov;
};

#endif // __RENDERWINDOW_H
