#pragma once

namespace Settings
{
	namespace JSON
	{
		class Holder : 
			public ISingleton<Holder>
		{
		public:
			bool Read();

		private:
		};
	}
}
