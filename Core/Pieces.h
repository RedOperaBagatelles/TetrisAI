#pragma once

#include "SFML/Graphics.hpp"

#include <vector>
#include <memory>

// 테트리스 조각의 종류를 나타내는 열거형
enum class PieceType { None, I, J, L, O, S, T, Z };

class Pieces
{
public:
	// 벽을 나타내는 사각형들을 반환하는 메소드
	static std::vector<std::shared_ptr<const sf::Drawable>> GetRenderWalls(int x, int y);

	// 해당 위치에 해당하는 PieceType의 렌더링용 사각형을 반환
	std::shared_ptr<const sf::RectangleShape> GetRenderOnePices(int x, int y, PieceType type, int offsetX, int offsetY);

	// 7 pieceRotateDatas, 4 rotations, 4x4 grid
	static const std::vector<std::vector<std::vector<std::vector<bool>>>> pieceRotateDatas;

private:
	// 각 PieceType에 해당하는 색상을 저장하는 배열
	static constexpr std::array<sf::Color, 8> colors =
	{
		sf::Color(197, 15, 31),
		sf::Color(97, 214, 214),
		sf::Color(0, 55, 218),
		sf::Color(255, 165, 0), // Orange
		sf::Color::Yellow,
		sf::Color(19, 161, 14),
		sf::Color(136, 23, 152),
		sf::Color(197, 15, 31)
	};

	static constexpr sf::Color wallColor = sf::Color(118, 118, 118);
	static constexpr int blockPadding = 2;	// 각 블록 사이의 간격 (픽셀 단위)
	static constexpr int blockSize = 20;	// 블록 사이즈
};

// 각 PieceType에 해당하는 4가지 회전 상태를 나타내는 4x4 그리드의 4차원 벡터
inline const std::vector<std::vector<std::vector<std::vector<bool>>>> Pieces::pieceRotateDatas =
{
	{
		{
			{ false, false, false, false },
			{ true,  true,  true,  true  },
			{ false, false, false, false },
			{ false, false, false, false }
		},
		{
			{ false, false, true, false },
			{ false, false, true, false },
			{ false, false, true, false },
			{ false, false, true, false }
		},
		{
			{ false, false, false, false },
			{ false, false, false, false },
			{ true,  true,  true,  true  },
			{ false, false, false, false }
		},
		{
			{ false, true, false, false },
			{ false, true, false, false },
			{ false, true, false, false },
			{ false, true, false, false }
		}
	},

	{
		{
			{ true,  false, false },
			{ true,  true,  true  },
			{ false, false, false }
		},
		{
			{ false, true,  true  },
			{ false, true,  false },
			{ false, true,  false }
		},
		{
			{ false, false, false },
			{ true,  true,  true  },
			{ false, false, true  }
		},
		{
			{ false, true,  false },
			{ false, true,  false },
			{ true,  true,  false }
		}
	},

	{
		{
			{ false, false, true },
			{ true,  true,  true  },
			{ false, false, false }
		},
		{
			{ false, true,  false },
			{ false, true,  false },
			{ false, true,  true  }
		},
		{
			{ false, false, false },
			{ true,  true,  true  },
			{ true,  false, false }
		},
		{
			{ true,  true,  false },
			{ false, true,  false },
			{ false, true,  false }
		}
	},

	{
		{
			{ true,  true  },
			{ true,  true  }
		},
		{
			{ true,  true  },
			{ true,  true  }
		},
		{
			{ true,  true  },
			{ true,  true  }
		},
		{
			{ true,  true  },
			{ true,  true  }
		}
	},
	{
		{
			{ false, true,  true  },
			{ true,  true,  false },
			{ false, false, false }
		},
		{
			{ false, true,  false },
			{ false, true,  true  },
			{ false, false, true  }
		},
		{
			{ false, false, false },
			{ false, true,  true  },
			{ true,  true,  false }
		},
		{
			{ true,  false, false },
			{ true,  true,  false },
			{ false, true,  false }
		}
	},

	{
		{
			{ false, true,  false },
			{ true,  true,  true  },
			{ false, false, false }
		},
		{
			{ false, true,  false },
			{ false, true,  true  },
			{ false, true,  false }
		},
		{
			{ false, false, false },
			{ true,  true,  true  },
			{ false, true,  false }
		},
		{
			{ false, true,  false },
			{ true,  true,  false },
			{ false, true,  false }
		}
	},

	{
		{
			{ true,  true,  false },
			{ false, true,  true  },
			{ false, false, false }
		},
		{
			{ false, false, true  },
			{ false, true,  true  },
			{ false, true,  false }
		},
		{
			{ false, false, false },
			{ true,  true,  false },
			{ false, true,  true  }
		},
		{
			{ false, true,  false },
			{ true,  true,  false },
			{ true,  false, false }
		}
	}
};