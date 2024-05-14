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
        //TODO : Union Dragon Support 
        if ( currentEntityPtr->GetName ( ).Contains ( "Ice" ) || currentEntityPtr->GetName ( ).Contains ( "Blue" ) ) {
            spell = Template ( bCString ( G3ICELANCE ) );
        }
        else if ( currentEntityPtr->GetName ( ).Contains ( "Stone" ) || currentEntityPtr->GetName ( ).Contains ( "Black" ) )
        {
            spell = Template ( bCString ( G3LIGHNINGBOLD ) );
        }
        else {
            spell = Template ( bCString ( G3FIREBALL ) );
        }
        stack = currentEntityPtr->Inventory.AssureItems(spell, 0, 1);
        retAdr = static_cast<DWORD>(Hook_NewMagicSystem.GetImmEdi<DWORD>());
        *(int*)(retAdr) = stack;
        return;
    case gESpecies_IceGolem:
        spell = Template(bCString( G3ICEBLOCK ));
        if ( !spell.IsValid() )
            spell = Template ( G3ICELANCE );
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

    switch (magetype) {
    case MageType_Firemage:
        if ( powerLevel >= 30 && GetScriptAdmin ( ).CallScriptFromScript ( "IsEvil" , &currentTarget , &None , 0 ) ) {
            spell = Template ( G3DESTROYEVIL );
            break;
        }
        if (magicpower == HIGH) {
            //if ( GE_DEBUG ) //std::cout << G3FIRERAIN << std::endl;
            spell = Template ( bCString ( G3FIRERAIN ) );
            break;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONFIREGOLEM << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONFIREGOLEM));
                break;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW) {
            if ( distance < ( GEFloat )725.0 ) {
                //std::cout << G3HEATWAVE << std::endl;
                spell = Template ( bCString ( G3HEATWAVE ) );
                break;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3FIREBALL << std::endl;
        spell = Template(bCString(G3FIREBALL));
        break;
    case MageType_Watermage:
        if (magicpower == HIGH) {
            //if (GE_DEBUG) //std::cout << G3HAILSTORM << std::endl;
            spell = Template(bCString(G3HAILSTORM));
            break;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONICEGOLEM << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONICEGOLEM));
                break;
            }
            spell = Template ( G3ICEBLOCK );
            break;
        }
        if (magicpower == LOW) {
            if ( distance < ( GEFloat )725.0 ) {
                //if ( GE_DEBUG ) //std::cout << G3FROSTWAVE << std::endl;
                spell = Template ( bCString ( G3FROSTWAVE ) );
                break;
            }
            if ( random <= 60 ) {
                spell = Template ( G3SLEEP );
                break;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3ICELANCE << std::endl;
        spell = Template(bCString(G3ICELANCE));
        break;
    case MageType_Blackmage:
        if (magicpower == HIGH) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONLIGHTNING << std::endl;
            spell = Template(bCString(G3SUMMONLIGHTNING));
            break;
        }
        if (magicpower == MID) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONDEMON << std::endl;
            if (!hasMember(currentEntityPtr)) {
                spell = Template(bCString(G3SUMMONDEMON));
                break;
            }
            magicpower = LOW;
        }
        if (magicpower == LOW ) {
                //if ( GE_DEBUG ) //std::cout << G3POISON << std::endl;
            if ( !hasMember ( currentEntityPtr ) ) {
                spell = Template ( bCString ( G3SUMMONSKELETON ) );
                break;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3LIGHNINGBOLD << std::endl;
        spell = Template(bCString( G3LIGHNINGBOLD ));
        break;
    default:
        if ( magicpower == HIGH ) {
            //if ( GE_DEBUG ) //std::cout << G3FIRERAIN << std::endl;
            spell = Template ( bCString ( G3METEOR ) );
            break;
        }
        if ( magicpower == MID ) {
            //if ( GE_DEBUG ) //std::cout << G3SUMMONGOLEM << std::endl;
            if ( !hasMember ( currentEntityPtr ) ) {
                spell = Template ( bCString ( G3SUMMONGOLEM ) );
                break;
            }
            magicpower = LOW;
        }
        if ( magicpower == LOW ) {
            //if ( GE_DEBUG ) //std::cout << G3SLEEP << std::endl;
            if ( random <= 60 ) {
                spell = Template ( bCString ( G3SLEEP ) );
                break;
            }
        }
        //if ( GE_DEBUG ) //std::cout << G3FIREBALL << std::endl;
        spell = Template ( bCString ( G3FIREBALL ) );
        break;
    }

    if ( !spell.IsValid ( ) ) {
        switch ( magetype ) {
        case MageType_Blackmage:
            spell = Template ( G3LIGHNINGBOLD );
            break;
        case MageType_Firemage:
            spell = Template ( G3FIREBALL );
            break;
        case MageType_Watermage:
            spell = Template ( G3ICELANCE );
            break;
        default:
            spell = Template ( G3FIREBALL );
        }
    }
    stack = currentEntityPtr->Inventory.AssureItems ( spell , 0 , 1 );
    retAdr = static_cast< DWORD >( Hook_NewMagicSystem.GetImmEdi<DWORD> ( ) );
    *( int* )( retAdr ) = stack;
    //return;
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

static mCFunctionHook Hook_AssessTarget;

GEInt AssessTarget ( gCScriptProcessingUnit* a_pSPU , Entity* a_pSelfEntity , Entity* a_pOtherEntity , GEU32 a_iArgs ) {
    INIT_SCRIPT_EXT ( Victim , Damager );
    if ( Victim == Entity::GetPlayer ( ) ) {
        return 0;
    }
    return Hook_AssessTarget.GetOriginalFunction( &AssessTarget )( a_pSPU , a_pSelfEntity , a_pOtherEntity , a_iArgs );
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

    // Fixes the AI Behaviour if the PC_Hero gets some MagicFunctions
    Hook_AssessTarget.Hook( GetScriptAdminExt ( ).GetScript ("AssessTarget")->m_funcScript, &AssessTarget, mCBaseHook::mEHookType_OnlyStack );

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
