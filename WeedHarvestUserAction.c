// Scripts/Game/UserActions/WeedHarvestUserAction.c
class WeedHarvestUserAction : ScriptedUserAction
{
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {
        WeedHarvestInteractionComponent harvestComp = WeedHarvestInteractionComponent.Cast(pOwnerEntity.FindComponent(WeedHarvestInteractionComponent));
        if (harvestComp)
        {
            harvestComp.HarvestWeed(pUserEntity);
        }
    }
    
    override bool GetActionNameScript(out string outName)
    {
        WeedHarvestInteractionComponent harvestComp = WeedHarvestInteractionComponent.Cast(GetOwner().FindComponent(WeedHarvestInteractionComponent));
        if (harvestComp)
        {
            outName = harvestComp.GetInteractionText();
        }
        else
        {
            outName = "Récolter";
        }
        return true;
    }
    
    override bool CanBeShownScript(IEntity user)
    {
        WeedHarvestInteractionComponent harvestComp = WeedHarvestInteractionComponent.Cast(GetOwner().FindComponent(WeedHarvestInteractionComponent));
        if (!harvestComp)
            return false;
            
        // Vérifier la distance
        vector ownerPos = GetOwner().GetOrigin();
        vector userPos = user.GetOrigin();
        float distance = vector.Distance(ownerPos, userPos);
        
        if (distance > 3.0) // Distance maximale d'interaction
            return false;
            
        return !harvestComp.HasBeenHarvested();
    }
    
    override bool CanBePerformedScript(IEntity user)
    {
        return CanBeShownScript(user);
    }
    
    override bool HasLocalEffectOnlyScript()
    {
        return false; // L'action doit être synchronisée
    }
}
