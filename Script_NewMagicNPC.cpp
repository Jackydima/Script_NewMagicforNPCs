#include "Script_NewMagicNPC.h"

#define GE_DEBUG 0

gSScriptInit & GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}
static mCFunctionHook Hook_NewMagicSystem;
static mCCallHook Hook_AI_StartCastPhase;


void GE_FASTCALL NewMagicSystem(Entity * currentEntityPtr)
{
    //if ( GE_DEBUG ) //std::cout << currentEntityPtr << "\n";
    if (currentEntityPtr == NULL) {
        //Call original?
        return;
    }

    GEInt stack = 0;
    DWORD retAdr = 0;

    int random = Entity::GetRandomNumber(1000);

    //std::cout << "Random: " << random << std::endl;

    bCString plunder5 = currentEntityPtr->Inventory.GetTreasureSet5().GetName();
    //std::cout << plunder5 << "\n";
    Entity currentTarget = currentEntityPtr->NPC.GetCurrentTarget();
    GEU32 NPC_Level = currentEntityPtr->NPC.GetProperty < PSNpc::PropertyLevel> ( );
    GEU32 NPC_LevelMax = currentEntityPtr->NPC.GetProperty < PSNpc::PropertyLevelMax> ( );
    GEU32 Player_Level = Entity::GetPlayer ( ).NPC.GetProperty<PSNpc::PropertyLevel> ( );
    //std::cout << "NPC_Level: " << NPC_Level << "\tLevelMax: " << NPC_LevelMax << "\tPlayerLevel: " << Player_Level << "\n";
    Template spell = Template();
    //std::cout << "Target: " << currentTarget.GetName() << "\n";

    if (!hasMember(currentEntityPtr) && random < 100) {
        bCString name = currentEntityPtr->GetName();
        //std::cout << name << std::endl;
        GEBool isSpecial = false;

        if (name == "Xardas") {
            spell = Template(bCString(G3SUMMONARMYOFDARKNESS));
            isSpecial = true;
        }
        else if (name == "Tizgar") {
            spell = Template(bCString(G3SUMMONDEMON));
            isSpecial = true;
        }
        else if (name == "Ningal") {
            spell = Template(bCString( G3SUMMONSKELETON ));
            isSpecial = true;
        }
        else if (name == "Runak") {
            spell = Template(bCString(G3SUMMONMONSTER));
            isSpecial = true;
        }
        else if (name == "Saturas") {
            spell = Template(bCString(G3SUMMONICEGOLEM));
            isSpecial = true;
        }
        if (isSpecial) {
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
    }

    //gESpecies species = (gESpecies)(*(int*)(*(DWORD*)(currentEntityPtr + 0x34)) + 0x34); // ENTITY -> NPC -> enum Species
    gESpecies species = currentEntityPtr->NPC.GetProperty<PSNpc::PropertySpecies>();

    //if ( GE_DEBUG ) //std::cout << "Species: " << species << std::endl;

    switch (species) {
    case gESpecies_Goblin:
        spell = Template(bCString(G3FIREBALL));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    case gESpecies_FireGolem:
    case gESpecies_Dragon:
        spell = Template(bCString(G3FIREBALL));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    case gESpecies_IceGolem:
        spell = Template(bCString(G3ICELANCE));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    }

    int healthPercantage = GetScriptAdmin().CallScriptFromScript(bCString("GetHitPointsPercent"),currentEntityPtr,0,0);
    //if ( GE_DEBUG ) //std::cout << "HealthP: " << healthPercantage << std::endl;
    
    if (healthPercantage < 70) {
        if (random < (75 - healthPercantage) * 10) {
            spell = Template(bCString(G3Healing));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
    }

    gESpecies targetSpecies = currentTarget.NPC.GetProperty<PSNpc::PropertySpecies>();
    //if ( GE_DEBUG ) //std::cout << "Target Species" << targetSpecies << std::endl;

    if (targetSpecies == gESpecies_FireGolem) {
        spell = Template(bCString(G3ICELANCE));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    }
    else if (targetSpecies == gESpecies_IceGolem) {
        spell = Template(bCString(G3FIREBALL));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    }
    GEFloat distance = currentEntityPtr->GetDistanceTo ( currentTarget ); // Edited
    //std::cout << "Distance: " << distance << "\n";
    Magicpower magicpower = Magicpower::NORMAL;
    //if ( GE_DEBUG ) //std::cout << "Normal MagicCaster!" << std::endl;

    MageType magetype = getMageType ( currentEntityPtr );

    GEU32 powerLevel = NPC_Level + Player_Level;
    //std::cout << "Powerlevel before Adjustments: " << powerLevel << "\n";
    if ( powerLevel > NPC_LevelMax ) powerLevel = NPC_LevelMax;

    if ((random >= 0 && random < 20) && powerLevel > 39) {
        magicpower = HIGH;
        //if ( GE_DEBUG ) //std::cout << "HIGH" << std::endl;
    }
    else if ((random < 40) && powerLevel > 34) {
        magicpower = MID;
        //if ( GE_DEBUG ) //std::cout << "MID" << std::endl;
    }
    else if ((random < 100) && powerLevel > 24 ) {
        magicpower = LOW;
        ////if ( GE_DEBUG ) //std::cout << "LOW" << std::endl;
    }

    if (plunder5 == G3ISFIREMAGE || magetype == MageType_Firemage ) {
        if (magicpower == HIGH) {
            //if ( GE_DEBUG ) //std::cout << G3FIRERAIN << std::endl;
            spell = Template(bCString(G3FIRERAIN));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONFIREGOLEM << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONFIREGOLEM));
                stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
                retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
                *(int*)(retAdr) = stack;
                return;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW) {
            if ( distance < ( GEFloat )725.0 ) {
                //std::cout << G3HEATWAVE << std::endl;
                spell = Template ( bCString ( G3HEATWAVE ) );
                stack = currentEntityPtr->Inventory.AssureItems ( spell , 0 , 1 );
                retAdr = static_cast< DWORD >( Hook_NewMagicSystem.GetImmEdi<DWORD> ( ) );
                *( int* )( retAdr ) = stack;
                return;
            }
        }

        //if ( GE_DEBUG ) //std::cout << G3FIREBALL << std::endl;
         spell = Template(bCString(G3FIREBALL));
         stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
         retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
         *(int*)(retAdr) = stack;
         return;
    }
    else if (plunder5 == G3ISWATERMAGE || magetype == MageType_Watermage ) {
        if (magicpower == HIGH) {
            if (GE_DEBUG) //std::cout << G3HAILSTORM << std::endl;
            spell = Template(bCString(G3HAILSTORM));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONICEGOLEM << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONICEGOLEM));
                stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
                retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
                *(int*)(retAdr) = stack;
                return;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW) {
            if ( distance < ( GEFloat )725.0 ) {
                //if ( GE_DEBUG ) //std::cout << G3FROSTWAVE << std::endl;
                spell = Template ( bCString ( G3FROSTWAVE ) );
                stack = currentEntityPtr->Inventory.AssureItems ( spell , 0 , 1 );
                retAdr = static_cast< DWORD >( Hook_NewMagicSystem.GetImmEdi<DWORD> ( ) );
                *( int* )( retAdr ) = stack;
                return;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3ICELANCE << std::endl;
        spell = Template(bCString(G3ICELANCE));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    }
    else if (plunder5 == G3ISBLACKMAGE || magetype == MageType_Blackmage ) {
        if (magicpower == HIGH) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONLIGHTNING << std::endl;
            spell = Template(bCString(G3SUMMONLIGHTNING));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONDEMON << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONDEMON));
                stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
                retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
                *(int*)(retAdr) = stack;
                return;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW) {
            if ( distance > 674 ) {
                //if ( GE_DEBUG ) //std::cout << G3POISON << std::endl;
                spell = Template ( bCString ( G3POISON ) );
                stack = currentEntityPtr->Inventory.AssureItems ( spell , 0 , 1 );
                retAdr = static_cast< DWORD >( Hook_NewMagicSystem.GetImmEdi<DWORD> ( ) );
                *( int* )( retAdr ) = stack;
                return;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3LIGHNINGBOLD << std::endl;
        spell = Template(bCString( G3LIGHNINGBOLD ));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    }
    else {
        if (magicpower == HIGH) {
            //if ( GE_DEBUG ) //std::cout << G3FIRERAIN << std::endl;
            spell = Template(bCString(G3METEOR));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONGOLEM << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONGOLEM));
                stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
                retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
                *(int*)(retAdr) = stack;
                return;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW) {
            //if ( GE_DEBUG ) //std::cout << G3SLEEP << std::endl;
            spell = Template(bCString(G3SLEEP));
            stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
            retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
            *(int*)(retAdr) = stack;
            return;
        }
        //if ( GE_DEBUG ) //std::cout << G3FIREBALL << std::endl;
        spell = Template(bCString(G3FIREBALL));
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;

    }
}

void GE_STDCALL AI_StartCastPhaseFix () 
{
    Entity* caster = (Entity*)(Hook_AI_StartCastPhase.GetImmEsi<GEInt>()+ 0x04);
    Entity currentItemRH = caster->Inventory.GetItemFromSlot ( gESlot_RightHand );
    Entity currentSpell;
    if ( currentItemRH.IsItem ( ) ) {
        currentSpell = currentItemRH.Item.GetSpell();
        Entity* target = ( Entity* )( Hook_AI_StartCastPhase.GetImmEsi<GEInt> ( ) + 0xb0 ); // Or .GetCurrentTarget()...
        caster->NPC.SetSpellTarget ( *target );
        caster->Interaction.SetSpell ( currentSpell );
    }
}

extern "C" __declspec( dllexport )
gSScriptInit const * GE_STDCALL ScriptInit( void )
{
    // Ensure that that Script_Game.dll is loaded.
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

    Hook_AI_StartCastPhase
        .Prepare ( RVA_ScriptGame ( 0x39806 ) , &AI_StartCastPhaseFix , mCBaseHook::mEHookType_Mixed , mCRegisterBase::mERegisterType_Esi ).InsertCall()
        .Hook();
    Hook_NewMagicSystem
        .Prepare(RVA_ScriptGame(0x49890), &NewMagicSystem, mCBaseHook::mEHookType_Mixed, mCRegisterBase::mERegisterType_Edi)
        .Hook();

    return &GetScriptInit();
}
//
// Entry Point
//

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID )
{
    switch( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        //AllocConsole();
        //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        ::DisableThreadLibraryCalls( hModule );
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
