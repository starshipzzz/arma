// Scripts/Game/Components/WeedHarvestInteractionComponent.c
[ComponentEditorProps(category: "Interaction", description: "Permet de récolter de la weed")]
class WeedHarvestInteractionComponentClass: ScriptComponentClass
{
}

class WeedHarvestInteractionComponent: ScriptComponent
{
    [Attribute("Récolter la weed", UIWidgets.EditBox, "Texte d'interaction affiché")]
    protected string m_sInteractionText;
    
    [Attribute("3.0", UIWidgets.SpinBox, "Distance maximale pour l'interaction", params: "0 100 0.1")]
    protected float m_fInteractionDistance;
    
    [Attribute("", UIWidgets.ResourcePickerThumbnail, "Prefab de l'item à donner", "et")]
    protected ResourceName m_sItemPrefab;
    
    [Attribute("1", UIWidgets.SpinBox, "Quantité d'items à donner", params: "1 10 1")]
    protected int m_iItemAmount;
    
    protected bool m_bHasBeenHarvested = false;
    protected ref array<IEntity> m_aNearbyPlayers = {};
    
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.FRAME);
    }
    
    override void EOnFrame(IEntity owner, float timeSlice)
    {
        super.EOnFrame(owner, timeSlice);
        
        // Vérifier la proximité des joueurs
        CheckPlayerProximity(owner);
    }
    
    protected void CheckPlayerProximity(IEntity owner)
    {
        if (m_bHasBeenHarvested)
            return;
            
        vector ownerPos = owner.GetOrigin();
        m_aNearbyPlayers.Clear();
        
        // Obtenir tous les joueurs du monde
        array<int> playerIds = {};
        GetGame().GetPlayerManager().GetPlayers(playerIds);
        
        foreach (int playerId : playerIds)
        {
            IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
            if (!playerEntity)
                continue;
                
            vector playerPos = playerEntity.GetOrigin();
            float distance = vector.Distance(ownerPos, playerPos);
            
            if (distance <= m_fInteractionDistance)
            {
                m_aNearbyPlayers.Insert(playerEntity);
            }
        }
    }
    
    void HarvestWeed(IEntity player)
    {
        if (m_bHasBeenHarvested)
        {
            ShowMessage(player, "Cette plante a déjà été récoltée!");
            return;
        }
        
        // Vérifier si le joueur est assez proche
        if (m_aNearbyPlayers.Find(player) == -1)
        {
            ShowMessage(player, "Vous êtes trop loin!");
            return;
        }
        
        // Obtenir l'inventaire du joueur
        SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
        if (!inventoryManager)
        {
            ShowMessage(player, "Impossible d'accéder à l'inventaire!");
            return;
        }
        
        // Créer les items à donner
        for (int i = 0; i < m_iItemAmount; i++)
        {
            if (m_sItemPrefab.IsEmpty())
            {
                ShowMessage(player, "Prefab d'item non configuré!");
                return;
            }
            
            Resource itemResource = Resource.Load(m_sItemPrefab);
            if (!itemResource)
            {
                ShowMessage(player, "Impossible de charger l'item!");
                return;
            }
            
            EntitySpawnParams spawnParams = EntitySpawnParams();
            spawnParams.TransformMode = ETransformMode.WORLD;
            spawnParams.Transform[3] = player.GetOrigin();
            
            IEntity itemEntity = GetGame().SpawnEntityPrefab(itemResource, GetOwner().GetWorld(), spawnParams);
            if (!itemEntity)
            {
                ShowMessage(player, "Impossible de créer l'item!");
                return;
            }
            
            // Essayer d'ajouter à l'inventaire
            bool success = inventoryManager.TryInsertItem(itemEntity);
            if (!success)
            {
                // Si l'inventaire est plein, placer l'item au sol près du joueur
                vector dropPos = player.GetOrigin() + (Math.RandomFloat01() - 0.5) * 2 * Vector(1, 0, 1);
                dropPos[1] = player.GetOrigin()[1];
                itemEntity.SetOrigin(dropPos);
                ShowMessage(player, "Inventaire plein! Item déposé au sol.");
            }
        }
        
        m_bHasBeenHarvested = true;
        ShowMessage(player, "Vous avez récolté de la weed!");
        
        // Changer l'apparence de la plante
        ChangeAppearanceAfterHarvest();
    }
    
    protected void ShowMessage(IEntity player, string message)
    {
        // Utiliser le système de chat pour afficher les messages
        PlayerController playerController = GetGame().GetPlayerController();
        if (!playerController)
            return;
            
        if (playerController.GetControlledEntity() == player)
        {
            // Utiliser Print pour debug ou le système de chat
            Print("[WEED] " + message, LogLevel.NORMAL);
            
            // Alternative: utiliser le système de HUD si disponible
            SCR_HUDManagerComponent hudManager = GetGame().GetHUDManager();
            if (hudManager)
            {
                // Afficher un message simple
                PrintFormat("[WEED] %1", message);
            }
        }
    }
    
    protected void ChangeAppearanceAfterHarvest()
    {
        IEntity owner = GetOwner();
        if (!owner)
            return;
            
        // Option 1: Déplacer la plante sous terre pour la "cacher"
        vector currentPos = owner.GetOrigin();
        currentPos[1] = currentPos[1] - 1.0; // Descendre de 1 mètre
        owner.SetOrigin(currentPos);
        
        // Option 2: Alternative - supprimer complètement la plante après un délai
        // GetGame().GetCallqueue().CallLater(DeletePlant, 2000, false, owner);
    }
    
    protected void DeletePlant(IEntity plantEntity)
    {
        if (plantEntity)
        {
            SCR_EntityHelper.DeleteEntityAndChildren(plantEntity);
        }
    }
    
    bool HasBeenHarvested()
    {
        return m_bHasBeenHarvested;
    }
    
    string GetInteractionText()
    {
        return m_sInteractionText;
    }
}
