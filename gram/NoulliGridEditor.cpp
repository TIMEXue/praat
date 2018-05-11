/* NoulliGridEditor.cpp
 *
 * Copyright (C) 2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "NoulliGridEditor.h"
#include "EditorM.h"

Thing_implement (NoulliGridEditor, TimeSoundEditor, 0);

#define SOUND_HEIGHT  0.2

/********** DRAWING AREA **********/

void structNoulliGridEditor :: v_draw () {
	NoulliGrid data = (NoulliGrid) our data;
	Graphics_Viewport viewport;
	if (our d_sound.data) {
		viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
		Graphics_setColour (our graphics.get(), Graphics_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), viewport);
		Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
	}
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, data -> tiers.size);
	for (integer itier = 1; itier <= data -> tiers.size; itier ++) {
		NoulliTier tier = data -> tiers.at [itier];
		double ymin = data -> tiers.size - itier, ymax = ymin + 1;
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			NoulliPoint point = tier -> points.at [ipoint];
			if (point -> xmax > our startWindow && point -> xmin < our endWindow) {
				double xmin = point -> xmin > our startWindow ? point -> xmin : our startWindow;
				double xmax = point -> xmax < our endWindow ? point -> xmax : our endWindow;
				double prob1 = 1.0, prob2;
				for (integer icategory = 1; icategory <= point -> numberOfCategories; icategory ++) {
					prob2 = prob1;
					prob1 -= point -> probabilities [icategory];
					Graphics_setColour (our graphics.get(), Graphics_cyclingBackgroundColour (icategory));
					Graphics_fillRectangle (our graphics.get(), xmin, xmax, ymin + prob1 * (ymax - ymin), ymin + prob2 * (ymax - ymin));
				}
			}
		}
		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		if (itier > 1) {
			Graphics_setLineWidth (our graphics.get(), 1.0);
			Graphics_line (our graphics.get(), our startWindow, ymax, our endWindow, ymax);
		}
	}
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	our v_updateMenuItems_file ();
}

void structNoulliGridEditor :: v_play (double a_tmin, double a_tmax) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

static void drawSelectionOrWindow (NoulliGridEditor me, double xmin, double xmax, double tmin, double tmax, const char32 *header) {
	NoulliGrid grid = (NoulliGrid) my data;
	for (integer itier = 1; itier <= grid -> tiers.size; itier ++) {
		Graphics_Viewport vp = Graphics_insetViewport (my graphics.get(), xmin, xmax,
			(grid -> tiers.size - itier + 0.0) / grid -> tiers.size * (1.0 - SOUND_HEIGHT),
			(grid -> tiers.size - itier + 1.0) / grid -> tiers.size * (1.0 - SOUND_HEIGHT));
		if (itier == 1) {
			Graphics_setColour (my graphics.get(), Graphics_BLACK);
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
			Graphics_text (my graphics.get(), 0.0, 1.0, header);
		}
		autoNoulliPoint average = NoulliGrid_average (grid, itier, tmin, tmax);
		integer winningCategory = NoulliPoint_getWinningCategory (average.get());
		if (winningCategory != 0 && average -> probabilities [winningCategory] > 1.0/3.0) {
			Graphics_setColour (my graphics.get(), Graphics_cyclingBackgroundColour (winningCategory));
			Graphics_fillEllipse (my graphics.get(), -0.985, +0.985, -0.985, +0.985);
			Graphics_setColour (my graphics.get(), Graphics_cyclingTextColour (winningCategory));
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
			Graphics_text (my graphics.get(), 0.0, 0.0, grid -> categoryNames [winningCategory]);
		} else {
			Graphics_setColour (my graphics.get(), Graphics_WHITE);
			Graphics_fillEllipse (my graphics.get(), -0.985, +0.985, -0.985, +0.985);
		}
		Graphics_resetViewport (my graphics.get(), vp);
	}
	Graphics_setColour (my graphics.get(), Graphics_BLACK);
}

void structNoulliGridEditor :: v_drawSelectionViewer () {
	Graphics_setWindow (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	Graphics_setColour (our graphics.get(), Graphics_WINDOW_BACKGROUND_COLOUR);
	Graphics_fillRectangle (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, 0.0, 0.5, our startSelection, our endSelection,
		our tmin == our tmax ? U"Cursor" : U"Selection");
	drawSelectionOrWindow (this, 0.5, 1.0, our startWindow, our endWindow, U"Window");
}

void structNoulliGridEditor :: v_drawRealTimeSelectionViewer (int phase, double time) {
	Graphics_setWindow (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, 0.0, 0.5, time - 2.0, time + 2.0, U"");
}

void NoulliGridEditor_init (NoulliGridEditor me, const char32 *title, NoulliGrid data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classNoulliGrid));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
}

autoNoulliGridEditor NoulliGridEditor_create (const char32 *title, NoulliGrid grid, Sound sound, bool ownSound) {
	try {
		autoNoulliGridEditor me = Thing_new (NoulliGridEditor);
		NoulliGridEditor_init (me.get(), title, grid, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NoulliGrid window not created.");
	}
}

/* End of file NoulliGridEditor.cpp */
