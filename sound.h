#pragma once


enum SoundID
{
	SndWool,
	SndStone,
	SndWood,
	SndBedDestruct,
	SndPlayerAttack,
	SndBowShoot,
	SndBurp,
	SndDrink,
	SndEat,
	SndExplode,
	SndGlass,
	SndHurt,
	SndLevelup,
	SndOrb,
	SndToast,
	SndWater,
	SndBell,
	SndIcechime,
	SndPling,
	SndItem,
	SndPlop,
	SndTeleport,
	SndFireball,
	SndFuse,
	SndEnter,
	SndLeave,
	SndHit,
	SndHitZombie,
	SndHitSkeleton,
	SndHitSilverfish,
	SndHitCreeper,
	SndHitVindicator,
	SndHitPillager,
	SndHitBlaze,
};
const vector<string> sound_names
{
	"wool.wav",
	"stone.wav",
	"wood.wav",
	"bed_destruction.wav",
	"attack.wav",
	"bow.wav",
	"burp.wav",
	"drink.wav",
	"eat.wav",
	"explode.wav",
	"glass.wav",
	"hurt.wav",
	"levelup.wav",
	"orb.wav",
	"toast.wav",
	"water.wav",
	"bell.wav",
	"icechime.wav",
	"pling.wav",
	"item.wav",
	"plop.wav",
	"teleport.wav",
	"fireball.wav",
	"fuse.wav",
	"door_open.wav",
	"door_close.wav",
	"hit.wav",
	"hit_zombie.wav",
	"hit_skeleton.wav",
	"hit_silverfish.wav",
	"hit_creeper.wav",
	"hit_vindicator.wav",
	"hit_pillager.wav",
	"hit_blaze.wav",
};

void SoundRaw(SoundID sid)
{
	sndPlaySound((g.audio_dir + sound_names.at(sid)).c_str(),
		SND_ASYNC | SND_FILENAME);
}
void Sound(SoundID sid)
{	//开独立线程
	thread thr([](string sname)
				{
					/*sndPlaySound((g.audio_dir + sname).c_str(),
						SND_SYNC | SND_FILENAME);*/

					CPlayer cp;
					if (!cp.Open((g.audio_dir + sname).c_str()))
					{
						//MessageBox(nullptr, sname.c_str(), "md", 0);
						MessageBeep(MB_ICONERROR);
						return;
					}
						
					DWORD dwLen = cp.GetLength();
					cp.Play();	//async

					api_sleep(dwLen);

					cp.Stop();
					cp.Close();
				},

			sound_names.at(sid));

	thr.detach();
}