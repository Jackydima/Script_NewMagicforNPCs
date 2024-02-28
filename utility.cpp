
#include "utility.h"


const GEInt fireMages_Size = 11;
bCString fireMages[] = {
	"Pyrokar",
	"Treslott",
	"Rakus",
	"Sebastian",
	"Karrypto",
	"KDF_01",
	"Aidan",
	"Altus",
	"Dargoth",
	"Pyran",
	"Milten"
};
const GEInt blackMages_Size = 25;
bCString blackMages[] = {
	"Ur-Gosh",
	"Grimboll",
	"Grok",
	"Grompel",
	"Kamak",
	"AssMage_01",
	"Orc_Shaman_01",
	"Orc_Shaman_02",
	"Orc_Shaman_03",
	"Orc_Raider_Shaman_01",
	"Orc_Raider_Shaman_02",
	"Orc_Raider_Shaman_03",
	"Orc_Raider_Shaman_03_Fireclan",
	"UrArash",
	"Orc_Raider_Shaman_Angir",
	"Xardas",
	"Amul",
	"Sigmor",
	"Tizgar",
	"Zombie_Yugul",
	"UndeadPriest",
	"QP_UndeadPriest",
	"Sigmor_UndeadPriest",
	"ZZ_Gotha_UndeadPriest",
	"Nafalem",
	"Delazar",
	"Nerusul",
	"Surus",
	"Ningal",
};
const GEInt waterMages_Size = 14;
const bCString waterMages[] = {
	"Runak",
	"Torn",
	"Porgan",
	"Druid_01",
	"Watermage_01",
	"Cronos",
	"Saturas",
	"Shakyor",
	"Vatras",
	"Myxir",
	"Riordian",
	"Merdarion",
	"Nefarius",
	"Bogir"
};

GEBool GE_STDCALL hasMember ( Entity* entityMem ) {
	bTValArray<eCEntity*>partyMember = entityMem->Party.GetMembers ( GEFalse );
	/*
	if (partyMember.GetArray()[0] == NULL) {
		//std::cout << "hasmember array??" << "\n";
		return GEFalse;
		}*/
	if ( !partyMember.IsEmpty() && entityMem->Party.GetPartyLeader() != *entityMem) {
		//std::cout << "Has Member = True";
		return GETrue;
	}
	//std::cout << "Has Member = False";
	return GEFalse;
}

GEBool isFireMage ( Entity* p_entity, bCString roleDescription ) {
	GEInt arrSize = sizeof ( fireMages ) / sizeof ( bCString );
	for ( GEInt i = 0; i < arrSize; i++ ) {
		if ( p_entity->GetName().Contains(fireMages[i]) ) {
			return GETrue;
		}
	}
	if ( roleDescription.Contains ( "Paladin" ) || roleDescription.Contains ( "Feuermagier" ) )
		return GETrue;
	return GEFalse;
}

GEBool isBlackMage ( Entity* p_entity, bCString roleDescription ) {
	GEInt arrSize = sizeof ( blackMages ) / sizeof ( bCString );
	for ( GEInt i = 0; i < arrSize; i++ ) {
		if ( p_entity->GetName ( ).Contains (blackMages[i]) ) {
			return GETrue;
		}
	}
	if ( roleDescription.Contains ( "Schwarzmagier" ) || roleDescription.Contains ( "Schamane" ) )
		return GETrue;
	return GEFalse;
}
GEBool isWaterMage ( Entity* p_entity, bCString roleDescription) {
	GEInt arrSize = sizeof ( waterMages ) / sizeof ( bCString );
	for ( GEInt i = 0; i < arrSize; i++ ) {
		if ( p_entity->GetName ( ).Contains ( waterMages[i] )) {
			return GETrue;
		}
	}
	if ( roleDescription.Contains ( "Wassermagier" ) || roleDescription.Contains ( "Druide" ) )
		return GETrue;
	return GEFalse;
}

MageType getMageType ( Entity* p_entity ) {
	GEChar* roleDescriptionPointer = ( GEChar* )*( DWORD* )( *( DWORD* )( &p_entity->NPC ) + 0x1C ); // Unsafe Access
	bCString roleDescription = "";
	//std::cout << "Address of entity: " << p_entity << "\n";
	//std::cout << "Address of entity: " << &p_entity->NPC << "\n";
	if ( roleDescriptionPointer != nullptr ) //std::cout << "Role?: " << roleDescriptionPointer << "\n";
	if ( roleDescriptionPointer != nullptr ) roleDescription = bCString ( roleDescriptionPointer );
	if ( isWaterMage ( p_entity, roleDescription ) ) {
		return MageType_Watermage;
	}
	else if ( isFireMage ( p_entity, roleDescription ) ) {
		return MageType_Firemage;
	}
	else if (isBlackMage( p_entity, roleDescription )) {
		return MageType_Blackmage;
	}
	return MageType_None;
}
