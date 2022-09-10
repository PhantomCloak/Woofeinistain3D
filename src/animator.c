#include "animator.h"

static animation_t animations[NUM_ANIMATION];
static animstate_t animationSteps[NUM_ANIMATION];
static const char* animationFolderNames[NUM_ANIMATION] = {
	"./anims/handgun",
	"./anims/smg"
};


void loadAnimations()
{
	struct dirent *entry;

	for(int i = 0; i < NUM_ANIMATION; i++)
	{
		int nodeAnimCount = 0;
		DIR* dir = opendir(animationFolderNames[i]);

        int size = strlen(animationFolderNames[i]) + sizeof(entry->d_name);
        char fullPathStrBuffer[size];

		if(dir == NULL)
		{
			printf("an error occured while reading the directiory");
			continue;
        }
        char *last = strrchr(animationFolderNames[i], '/');
        animation_t *anim = &animations[i];
        anim->name = last+1;

        char animationFiles[32][64]; // these paths relative anyways
        int animFilesCount = 0;
        
        // iterate png's in directory
		while((entry = readdir(dir)))
		{
			if(entry->d_name[0] == '.')
                continue;
			if(strcmp(entry->d_name,".") == 0)
				continue;
			if(strcmp(entry->d_name,"..") == 0)
				continue;

            strcat(fullPathStrBuffer, animationFolderNames[i]);
            strcat(fullPathStrBuffer, "/");
            strcat(fullPathStrBuffer, &entry->d_name[0]);

            strcpy(animationFiles[animFilesCount], fullPathStrBuffer);
            animFilesCount++;
            fullPathStrBuffer[0] = '\0';
        }

        // sort by name
        qsort(animationFiles, animFilesCount, 64, (int (*)(const void *, const void *))strcmp); 
        closedir(dir);

        // index animations
        for(int i = 0; i < animFilesCount; i++)
        {
            upng_t* upng;
            upng = upng_new_from_file(animationFiles[i]);

            if(upng == NULL)
            {
                printf("Error loading anim texture file %s\n", animationFiles[i]);
                continue;
            }

            upng_decode(upng);

            if(upng_get_error(upng) != UPNG_EOK)
            {
                printf("Error loading anim texture %s\n", animationFiles[i]);
                continue;
            }

            clip_t clip = {
                .texture = upng
            };

            anim->clips[anim->clipCount] = clip;
            anim->clipCount++;
        }
    }

    printf("");
}


animation_t *getAnimationHandle(const char* name)
{
    for(int i = 0; i < NUM_ANIMATION; i++)
    {
        if(strcmp(animations[i].name, name) == 0)
        {
            return &animations[i];
        }
    }

    printf("Error loading animation %s", name);
    return NULL;
}

void animateTick()
{
    for(int i = 0; i < NUM_ANIMATION; i++)
    {
        animstate_t *animState = &animationSteps[i];

        if(animState == NULL) 
            continue;
        if(animState->target == NULL)
            continue;
    }
}
