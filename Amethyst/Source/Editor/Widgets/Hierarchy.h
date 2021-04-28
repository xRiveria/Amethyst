#pragma once
#include "../Widget.h"

namespace Amethyst
{
	class Hierarchy : public Widget
	{
	public:
		Hierarchy(Editor* editor);
		void OnVisibleTick() override;

	private:
		void ShowTree();
		void OnTreeBegin();
		void OnTreeEnd();
		void AddTreeEntity();

		//Misc (Popups)
		

		//Context Menu Actions
	};
}