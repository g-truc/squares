#include "test.hpp"
#include <glm/gtc/noise.hpp>
#include <glm/gtx/color_space.hpp>

using namespace tinyxml2;

namespace
{
	char const* VERT_SHADER_SOURCE("texture-float.vert");
	char const* FRAG_SHADER_SOURCE("texture-float.frag");
	char const* DATABASE_SOURCE("squares.xml");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	float const Scale(0.8f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-Scale, -Scale), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(+Scale, -Scale), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(+Scale, +Scale), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-Scale, +Scale), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader

	static std::size_t const Set1[][64] =
	{
		{
			4, 0, 1, 2, 1, 7, 0, 7,
			7, 1, 0, 5, 6, 7, 7, 3,
			1, 5, 2, 5, 7, 2, 6, 1,
			1, 3, 3, 3, 0, 2, 4, 0,
			7, 5, 0, 6, 1, 2, 0, 3,
			7, 2, 4, 1, 7, 3, 4, 3,
			4, 3, 5, 1, 5, 2, 3, 6,
			7, 2, 2, 2, 5, 5, 6, 6,
		},
		{
			0, 2, 3, 4, 1, 6, 5, 7,
			5, 4, 0, 5, 3, 7, 2, 1,
			6, 7, 1, 2, 5, 4, 3, 0,
			1, 5, 4, 3, 7, 0, 6, 2,
			3, 6, 7, 0, 2, 1, 4, 5,
			2, 0, 5, 1, 4, 3, 7, 6,
			4, 1, 2, 7, 6, 5, 0, 3,
			7, 3, 6, 5, 0, 2, 1, 4,
		},
		{
			2, 3, 4, 5, 7, 2, 7, 5,
			6, 7, 1, 3, 6, 6, 2, 0,
			6, 7, 5, 7, 6, 6, 2, 4,
			6, 3, 1, 7, 6, 6, 7, 1,
			7, 0, 4, 3, 2, 2, 5, 1,
			0, 1, 4, 5, 3, 5, 4, 1,
			0, 0, 5, 5, 0, 4, 1, 1,
			2, 2, 7, 5, 4, 1, 1, 4,
		},
		{
			0, 7, 6, 7, 3, 6, 7, 3,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			1, 4, 0, 4, 2, 0, 4, 2,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			2, 6, 3, 6, 1, 3, 6, 1,
			6, 1, 5, 1, 4, 5, 1, 4,
		}
	};

	static std::size_t const Set2[][64] =
	{
		{
			4, 0, 1, 0, 7, 1, 4, 1,
			3, 7, 4, 7, 3, 6, 3, 0,
			1, 6, 5, 5, 6, 1, 7, 2,
			0, 5, 5, 4, 0, 2, 2, 1,
			7, 5, 7, 7, 0, 1, 4, 7,
			0, 7, 6, 7, 2, 3, 4, 6,
			3, 1, 3, 1, 0, 5, 3, 1,
			3, 1, 1, 0, 5, 5, 5, 3,
		},
		{
			7, 3, 6, 0, 5, 4, 1, 2,
			4, 5, 2, 1, 7, 3, 6, 0,
			3, 6, 1, 4, 2, 0, 7, 5,
			2, 0, 7, 5, 6, 1, 4, 3,
			0, 7, 3, 6, 4, 5, 2, 1,
			5, 1, 4, 2, 3, 6, 0, 7,
			6, 2, 0, 3, 1, 7, 5, 4,
			1, 4, 5, 7, 0, 2, 3, 6
		},
		{
			2, 3, 4, 5, 7, 2, 7, 5,
			6, 7, 1, 3, 6, 6, 2, 0,
			6, 7, 5, 7, 6, 6, 2, 4,
			6, 3, 1, 7, 6, 6, 7, 1,
			7, 0, 4, 3, 2, 2, 5, 1,
			0, 1, 4, 5, 3, 5, 4, 1,
			0, 0, 5, 5, 0, 4, 1, 1,
			2, 2, 7, 5, 4, 1, 1, 4,
		},
		{
			0, 7, 6, 7, 3, 6, 7, 3,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			1, 4, 0, 4, 2, 0, 4, 2,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			2, 6, 3, 6, 1, 3, 6, 1,
			6, 1, 5, 1, 4, 5, 1, 4,
		}
	};

	static std::size_t const Set3[][64] =
	{
		{
			0, 4, 3, 4, 7, 2, 6, 5,
			3, 3, 5, 1, 2, 0, 3, 1,
			0, 2, 3, 2, 5, 3, 7, 4,
			3, 2, 5, 4, 3, 7, 3, 6,
			1, 7, 6, 1, 0, 6, 2, 3,
			2, 7, 1, 4, 5, 0, 4, 5,
			6, 6, 3, 3, 4, 5, 5, 4,
			1, 6, 4, 4, 3, 4, 3, 3,
		},
		{
			6, 0, 7, 3, 2, 4, 5, 1,
			4, 1, 2, 5, 0, 7, 3, 6,
			7, 3, 5, 6, 4, 1, 0, 2,
			2, 7, 4, 0, 1, 3, 6, 5,
			0, 5, 1, 7, 6, 2, 4, 3,
			1, 2, 3, 4, 5, 6, 7, 0,
			5, 4, 6, 2, 3, 0, 1, 7,
			3, 6, 0, 1, 7, 5, 2, 4
		},
		{
			2, 3, 4, 5, 7, 2, 7, 5,
			6, 7, 1, 3, 6, 6, 2, 0,
			6, 7, 5, 7, 6, 6, 2, 4,
			6, 3, 1, 7, 6, 6, 7, 1,
			7, 0, 4, 3, 2, 2, 5, 1,
			0, 1, 4, 5, 3, 5, 4, 1,
			0, 0, 5, 5, 0, 4, 1, 1,
			2, 2, 7, 5, 4, 1, 1, 4,
		},
		{
			0, 7, 6, 7, 3, 6, 7, 3,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			1, 4, 0, 4, 2, 0, 4, 2,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			2, 6, 3, 6, 1, 3, 6, 1,
			6, 1, 5, 1, 4, 5, 1, 4,
		}
	};

	static std::size_t const Set4[][64] =
	{
		{
			2, 4, 7, 5, 0, 1, 6, 0,
			0, 5, 7, 0, 6, 1, 0, 3,
			3, 7, 3, 0, 6, 6, 4, 1,
			7, 5, 0, 4, 6, 0, 7, 2,
			4, 3, 6, 6, 5, 3, 6, 0,
			3, 2, 2, 1, 6, 0, 1, 2,
			2, 4, 6, 4, 1, 7, 6, 3,
			2, 5, 7, 6, 2, 5, 6, 1,
		},
		{
			6, 7, 2, 3, 1, 5, 0, 4,
			4, 3, 0, 5, 7, 6, 1, 2,
			1, 5, 7, 6, 2, 4, 0, 3,
			0, 2, 1, 4, 3, 7, 6, 5,
			3, 4, 6, 2, 5, 0, 7, 1,
			7, 0, 5, 1, 4, 3, 2, 6,
			2, 6, 4, 7, 0, 1, 5, 3,
			5, 1, 3, 0, 6, 2, 4, 7,
		},
/*
		{
			0, 7, 1, 5, 3, 6, 2, 4,
			3, 6, 0, 1, 5, 2, 4, 7,
			4, 2, 3, 7, 0, 1, 5, 6,
			5, 3, 6, 4, 2, 7, 0, 1,
			7, 0, 2, 3, 1, 4, 6, 5,
			6, 5, 4, 0, 7, 3, 1, 2,
			2, 1, 5, 6, 4, 0, 7, 3,
			1, 4, 7, 2, 6, 5, 3, 0,
		},
*/
		{
			2, 3, 4, 5, 7, 2, 7, 5,
			6, 7, 1, 3, 6, 6, 2, 0,
			6, 7, 5, 7, 6, 6, 2, 4,
			6, 3, 1, 7, 6, 6, 7, 1,
			7, 0, 4, 3, 2, 2, 5, 1,
			0, 1, 4, 5, 3, 5, 4, 1,
			0, 0, 5, 5, 0, 4, 1, 1,
			2, 2, 7, 5, 4, 1, 1, 4,
		},
		{
			0, 7, 6, 7, 3, 6, 7, 3,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			1, 4, 0, 4, 2, 0, 4, 2,
			2, 6, 3, 6, 1, 3, 6, 1,
			3, 5, 4, 5, 0, 4, 5, 0,
			2, 6, 3, 6, 1, 3, 6, 1,
			6, 1, 5, 1, 4, 5, 1, 4,
		}
	};
}//namespace

class squares : public framework
{
public:
	squares(int argc, char* argv[]) :
		framework(argc, argv, "Squares", framework::CORE, 4, 5, glm::uvec2(1280)),
		VertexArrayName(0),
		ProgramName(0),
		TextureName(0)
	{}

private:
	typedef std::vector<glm::u8vec4> palette;
	std::vector<palette> Palettes;

	struct draw
	{
		std::size_t Column;
		std::size_t Row;
		std::size_t ColorIndex;
	};

	struct component
	{
		std::string Label;
		std::size_t PaletteIndex;
		std::vector<draw> Draws;
	};

	std::vector<component> Components;

	void load_database(char const* Filename)
	{
		XMLDocument Document;
		Document.LoadFile(Filename);

		XMLElement* SquaresElement = Document.FirstChildElement("squares");

		for (XMLElement* PaletteElement = SquaresElement->FirstChildElement("palette"); PaletteElement; PaletteElement = PaletteElement->NextSiblingElement("palette"))
		{
			std::size_t const PaletteIndex = PaletteElement->IntAttribute("index");
			if (PaletteIndex >= Palettes.size())
				Palettes.resize(PaletteIndex + 1);

			palette Palette;
			for (XMLElement* ColorElement = PaletteElement->FirstChildElement("color"); ColorElement; ColorElement = ColorElement->NextSiblingElement("color"))
			{
				std::size_t const ColorIndex = ColorElement->IntAttribute("index");
				if (ColorIndex >= Palette.size())
					Palette.resize(ColorIndex + 1);

				Palette[ColorIndex].r = static_cast<unsigned char>(ColorElement->IntAttribute("r"));
				Palette[ColorIndex].g = static_cast<unsigned char>(ColorElement->IntAttribute("g"));
				Palette[ColorIndex].b = static_cast<unsigned char>(ColorElement->IntAttribute("b"));
				Palette[ColorIndex].a = static_cast<unsigned char>(ColorElement->IntAttribute("a"));
			}

			Palettes[PaletteIndex] = Palette;
		}

		for (XMLElement* ComponentElement = SquaresElement->FirstChildElement("component"); ComponentElement; ComponentElement = ComponentElement->NextSiblingElement("component"))
		{
			component Component;
			Component.PaletteIndex = ComponentElement->IntAttribute("palette-index");
			Component.Label = ComponentElement->Attribute("label");

			for (XMLElement* DrawElement = ComponentElement->FirstChildElement("draw"); DrawElement; DrawElement = DrawElement->NextSiblingElement("draw"))
			{
				draw Draw;
				Draw.Column = DrawElement->IntAttribute("column");
				Draw.Row = DrawElement->IntAttribute("row");
				Draw.ColorIndex = DrawElement->IntAttribute("color-index");
				Component.Draws.push_back(Draw);
			}

			this->Components.push_back(Component);
		}
	}

	std::array<GLuint, buffer::MAX> BufferName;
	GLuint VertexArrayName;
	GLuint ProgramName;
	GLuint TextureName;
	GLint UniformLayer;

	bool init_program()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if (Validated)
		{
			glUniformBlockBinding(ProgramName, glGetUniformBlockIndex(ProgramName, "transform"), semantic::uniform::TRANSFORM0);

			glUseProgram(ProgramName);
			glUniform1i(glGetUniformLocation(ProgramName, "Diffuse"), 0);
			this->UniformLayer = glGetUniformLocation(ProgramName, "Layer");
			glUseProgram(0);
		}

		return Validated;
	}

	bool init_buffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	typedef std::array<glm::i8vec3, 8 * 8> color_chart;

	glm::vec3 contrast_saturation_brightness(glm::vec3 color, float brt, float sat, float con)
	{
		const glm::vec3 LumCoeff = glm::vec3(0.2125, 0.7154, 0.0721);

		glm::vec3 brtColor = color * brt;
		glm::vec3 intensity = glm::vec3(dot(brtColor, LumCoeff));
		glm::vec3 satColor = glm::mix(intensity, brtColor, sat);
		glm::vec3 conColor = glm::mix(glm::vec3(0.5), satColor, con);
		return conColor;
	}

	color_chart build_color_chart(glm::u8vec3 const Color[], std::size_t const Component[])
	{
		std::array<glm::i8vec3, 8 * 8> Data;

		for (std::size_t j = 0; j < 8; ++j)
		for (std::size_t i = 0; i < 8; ++i)
		{
			std::size_t const TexelIndex(i + j * 8);
			std::size_t const ColorIndex(Component[TexelIndex]);
			assert(ColorIndex >= 0 && ColorIndex <= 7);
			Data[TexelIndex] = Color[ColorIndex];
		}

		return Data;
	}

	bool init_texture_old()
	{
		std::size_t const Size(8);

		static glm::u8vec3 const ColorSRGB[] =
		{
			glm::u8vec3(225,   0, 152), // Rhodamine Red C
			glm::u8vec3(239,  51,  64), // Red 032 C
			glm::u8vec3(254,  80,   0), // Orange 021 C
			glm::u8vec3(254, 221,   0), // Yellow C
			glm::u8vec3(  0, 172, 140), // Green C
			glm::u8vec3(  0, 131, 195), // Process Blue C
			glm::u8vec3( 63,  67, 173), // Blue 072 C
			glm::u8vec3(187,  41, 187)  // Purple C
		};

		static glm::u8vec3 const ColorInstance2Comp3[] =
		{
			glm::u8vec3(239,  51,  64), // Red 032 C
			glm::u8vec3(0, 132, 61),  // 348 C (Green)
			glm::u8vec3(214,  37, 152), // Pink C
			glm::u8vec3(0, 133, 202),  // Process Blue C
			glm::u8vec3(255, 215,   0), // Yellow 012 C
			glm::u8vec3(78,  0, 142),  // Medium Purple C
			glm::u8vec3(254,  80,   0), // Orange 021 C
			glm::u8vec3(16,   6, 159), // Blue 072 C
		};

		static glm::u8vec3 const ColorGNI[] =
		{
			glm::u8vec3(214,  37, 152), // Pink C
			//glm::u8vec3(225,   0, 152), // Rhodamine Red C
			//glm::u8vec3(249,  56,  34), // Bright Red 032 C
			glm::u8vec3(239,  51,  64), // Red 032 C
			//glm::u8vec3(255,  94,   0), // Bright Orange C
			glm::u8vec3(254,  80,   0), // Orange 021 C
			glm::u8vec3(255, 215,   0), // Yellow 012 C
			//glm::u8vec3(0, 171, 132),  // Green C
			glm::u8vec3(0, 132, 61),  // 348 C (Green)
			glm::u8vec3(0, 133, 202),  // Process Blue C
			glm::u8vec3(16,   6, 159), // Blue 072 C
			glm::u8vec3( 78,  0, 142)  // Medium Purple C
		};

		static glm::u8vec3 const Color[] =
		{
			glm::u8vec3(214,  37, 152),		// 0, Pink C
			glm::u8vec3(239,  51,  64),		// 1, Red 032 C
			glm::u8vec3(254,  80,   0),		// 2, Orange 021 C
			glm::u8vec3(255, 215,   0),		// 3, Yellow 012 C
			glm::u8vec3(0, 132, 61),		// 4, 348 C (Green)
			glm::u8vec3(0, 133, 202),		// 5, Process Blue C
			glm::u8vec3(16,   6, 159),		// 6, Blue 072 C
			glm::u8vec3(78,  0, 142)		// 7, Medium Purple C
		};

		static glm::u8vec3 const ColorOrange[] =
		{
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.65f, 1.0f)) * 255.0f,		// 0
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.70f, 1.0f)) * 255.0f,		// 1
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.75f, 1.0f)) * 255.0f,		// 2
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.80f, 1.0f)) * 255.0f,		// 3
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.85f, 1.0f)) * 255.0f,		// 4
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.90f, 1.0f)) * 255.0f,		// 5
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.95f, 1.0f)) * 255.0f,		// 6
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 1.00f, 1.0f)) * 255.0f,		// 7
		};

		static glm::u8vec3 const ColorOrange2[] =
		{
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.90f, 1.0f)) * 255.0f,		// 3
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.90f, 1.0f)) * 255.0f,		// 3
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.90f, 1.0f)) * 255.0f,		// 3
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 0.90f, 1.0f)) * 255.0f,		// 3
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 1.00f, 1.0f)) * 255.0f,		// 7
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 1.00f, 1.0f)) * 255.0f,		// 7
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 1.00f, 1.0f)) * 255.0f,		// 7
			glm::rgbColor(glm::hsvColor(glm::vec3(254,  80,   0) / 255.0f) * glm::vec3(1.0f, 1.00f, 1.0f)) * 255.0f,		// 7
		};

		static glm::u8vec3 const ColorOrange3[] =
		{
			glm::saturation(0.30f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 0
			glm::saturation(0.40f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 1
			glm::saturation(0.50f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 2
			glm::saturation(0.60f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 3
			glm::saturation(0.70f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 4
			glm::saturation(0.80f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 5
			glm::saturation(0.90f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 6
			glm::saturation(1.00f, glm::vec3(255,  128,   0) / 255.0f) * 255.0f,		// 7
		};

		float const Scale = 0.1f;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &TextureName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, Size, Size, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		// Instance 3
		{
			color_chart const Data1 = build_color_chart(Color, Set1[0]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 0,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data1[0]);

			color_chart const Data2 = build_color_chart(Color, Set1[1]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 1,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data2[0]);

			color_chart const Data3 = build_color_chart(Color, Set1[2]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 2,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data3[0]);

			color_chart const Data4 = build_color_chart(Color, Set1[3]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 3,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data4[0]);
		}

		// Instance 4
		{
			color_chart const Data1 = build_color_chart(Color, Set2[0]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 4,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data1[0]);

			color_chart const Data2 = build_color_chart(Color, Set2[1]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 5,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data2[0]);

			color_chart const Data3 = build_color_chart(Color, Set2[2]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 6,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data3[0]);

			color_chart const Data4 = build_color_chart(Color, Set2[3]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 7,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data4[0]);
		}

		// Instance 5
		{
			color_chart const Data1 = build_color_chart(Color, Set3[0]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 8,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data1[0]);

			color_chart const Data2 = build_color_chart(Color, Set3[1]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 9,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data2[0]);

			color_chart const Data3 = build_color_chart(Color, Set3[2]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 10,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data3[0]);

			color_chart const Data4 = build_color_chart(Color, Set3[3]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 11,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data4[0]);
		}

		// Instance 6
		{
			color_chart const Data1 = build_color_chart(Color, Set4[0]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 12,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data1[0]);

			color_chart const Data2 = build_color_chart(Color, Set4[1]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 13,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data2[0]);

			color_chart const Data3 = build_color_chart(Color, Set4[2]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 14,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data3[0]);

			color_chart const Data4 = build_color_chart(Color, Set4[3]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, 15,
				static_cast<GLsizei>(Size), static_cast<GLsizei>(Size), 1,
				GL_RGB, GL_UNSIGNED_BYTE, &Data4[0]);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	bool init_texture()
	{
		std::size_t const Size(8);

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &this->TextureName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage3D(this->TextureName, 1, GL_RGBA8, Size, Size, static_cast<GLsizei>(this->Components.size()));

		glm::u8vec4 const ClearColor(0, 0, 0, 255);
		glClearTexImage(this->TextureName, 0, GL_RGBA, GL_UNSIGNED_BYTE, &ClearColor[0]);

		for(std::size_t ComponentIndex = 0; ComponentIndex < this->Components.size(); ++ComponentIndex)
		{
			component const& Component = this->Components[ComponentIndex];
			palette const& Palette = this->Palettes[Component.PaletteIndex];

			for(std::size_t DrawIndex = 0; DrawIndex < Component.Draws.size(); ++DrawIndex)
			{
				draw const& Draw = Component.Draws[DrawIndex];
				glm::u8vec4 const& SquareColor = Palette[Draw.ColorIndex];
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
					static_cast<GLint>(Draw.Row), static_cast<GLint>(Draw.Column), static_cast<GLint>(ComponentIndex),
					1, 1, 1,
					GL_RGBA, GL_UNSIGNED_BYTE, &SquareColor[0]);
			}
		}


		return true;
	}

	bool init_vertex_array()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
		glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(semantic::attr::POSITION);
		glEnableVertexAttribArray(semantic::attr::TEXCOORD);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated = true;

		this->load_database((getDataDirectory() + DATABASE_SOURCE).c_str());

		if(Validated)
			Validated = init_texture();
		if(Validated)
			Validated = init_program();
		if(Validated)
			Validated = init_buffer();
		if(Validated)
			Validated = init_vertex_array();

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = reinterpret_cast<glm::mat4*>(glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
			*Pointer = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glUseProgram(ProgramName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		return Validated;
	}

	bool end()
	{
		glDeleteProgram(ProgramName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(1, &TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::uvec2 const WindowSize(this->getWindowSize());

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		std::size_t const Width = 2;
		std::size_t const Height = 2;

		for(std::size_t y = 0; y < Height; ++y)
		for(std::size_t x = 0; x < Width; ++x)
		{
			glViewport(x * WindowSize.x / Width, y * WindowSize.y / Height, WindowSize.x / Width, WindowSize.y / Height);
			glUniform1i(UniformLayer, x + y * Width);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0);
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	squares Squares(argc, argv);
	Error += Squares();

	return Error;
}
