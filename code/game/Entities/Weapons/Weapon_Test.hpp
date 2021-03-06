#pragma once

namespace Entities
{
	class Weapon_Test final : public BaseWeapon
	{
	public:
		DeclareEntityClass();

		WeaponInfo	GetWeaponInfo() override;
		uint16_t	GetWeaponFlags() { return WFNone; }

		void		PrimaryAttack() override;
	};
}
