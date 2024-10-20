#include "include/position.h"
#include "include/board.h"
#include "include/constants.h"
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <iostream>

constexpr uint8_t HASH_IDX(Piece p) { return ((p > 8) ? p - 3 : p - 1); }

static const uint64_t HASHTABLE[12][64] = {
  {2605279972809034682ULL,  17019599012922847020ULL, 3054446656405098991ULL,
   9742036992642402681ULL,  7544710995780272403ULL,  9878784248969637120ULL,
   13749417686666270167ULL, 8967627540819818507ULL,  13662340049155921060ULL,
   1295751681899932285ULL,  13667983280612596428ULL, 1870554172022173220ULL,
   12406463054321478874ULL, 14046549610407462811ULL, 5836600468456595467ULL,
   14693036224299929843ULL, 17973103760988972468ULL, 8632757978165105056ULL,
   9405486877880649912ULL,  8871434355303455227ULL,  2925861020775576833ULL,
   16048996219797205924ULL, 1835415094435654257ULL,  837964794577040455ULL,
   16782712259314610657ULL, 7981712260863874611ULL,  6259508189695849252ULL,
   712251991435734345ULL,   14705648289117595685ULL, 1926071881608814209ULL,
   6976629025526557963ULL,  5764251123212117610ULL,  16115787344638446008ULL,
   513143983616765780ULL,   8301090867869094303ULL,  2079039829150706964ULL,
   6867231121727959660ULL,  3680915768880119409ULL,  2097958750716768408ULL,
   9483584488292268640ULL,  15595116804370585174ULL, 10005530463623818949ULL,
   5506829187815284075ULL,  13423561349686392743ULL, 16898340319654972465ULL,
   14526263354511400701ULL, 13795998800349690649ULL, 4001798697874284868ULL,
   307318125030066191ULL,   15480634807833083026ULL, 499055474893625000ULL,
   8870979124570336436ULL,  7574087920210272383ULL,  610833785348012105ULL,
   7427663570376243928ULL,  2097361942599607714ULL,  14974825478479920655ULL,
   9286433101227373588ULL,  13626508775240087661ULL, 10198753965434832495ULL,
   2173587816862849336ULL,  12282595694747560087ULL, 5514418481410847522ULL,
   8362955362647773013ULL},
  {15922693056400704897ULL, 8920400248571768462ULL,  12921016342789681166ULL,
   3321894021143472788ULL,  13239050913120225640ULL, 3915071094009739318ULL,
   3074130233703257603ULL,  922489315159473378ULL,   9362570660820040870ULL,
   5658467041975192293ULL,  2986290287017077033ULL,  4154710309583400419ULL,
   14533614341235903157ULL, 8378424929222831654ULL,  2303420286386266937ULL,
   6762750661569486687ULL,  14409059382914680274ULL, 13840518526071436487ULL,
   17757315665634987032ULL, 13223594116488446272ULL, 6078542196385210064ULL,
   7243297622459159351ULL,  2580100709630389067ULL,  12464965083888239623ULL,
   12248266999492667969ULL, 7504313437202048692ULL,  4960411350541612124ULL,
   2017523017643240207ULL,  607695745979214376ULL,   9553792729496125865ULL,
   8574829197710239261ULL,  15173705255761366526ULL, 16349469606461673673ULL,
   2121024632171428374ULL,  3091922893870183043ULL,  14155313614926284982ULL,
   4701266551374441010ULL,  147978837132521945ULL,   11116710011545200630ULL,
   1343770497970649807ULL,  12134831435863347769ULL, 5344910122870164634ULL,
   690318610984818826ULL,   1833708603225117559ULL,  1829305036518701734ULL,
   12884892394803071465ULL, 11684469367035424249ULL, 11638711986668901225ULL,
   339821726155187213ULL,   15198410879450190679ULL, 5063910268730783025ULL,
   16196235935262122843ULL, 6368296681268266915ULL,  2318313473398843953ULL,
   12569688361882768719ULL, 8183641694214659502ULL,  5401889232887015012ULL,
   7447123300594726643ULL,  150756716454964028ULL,   16015748869135049895ULL,
   14364465916271972118ULL, 11029045894723392387ULL, 4938208412515678673ULL,
   11913029605956797832ULL},
  {8244962820272760186ULL,  2785201302770414964ULL,  8973183107609319052ULL,
   1369741269894088210ULL,  13030575956485130857ULL, 9299857221759205415ULL,
   2205337915492249667ULL,  12159282593041390159ULL, 15872560051772908290ULL,
   17061418964264870804ULL, 18137795737054224628ULL, 16905839662013017028ULL,
   18170953173047808215ULL, 7299137258200166547ULL,  12512776703230594976ULL,
   12354519666907600409ULL, 17235805072841923893ULL, 1923221732512364364ULL,
   9001364854041330890ULL,  12544442147826533173ULL, 18272577151766404752ULL,
   7110641620135768134ULL,  14904196135899521690ULL, 10270549280083049652ULL,
   2589356537388279429ULL,  9749963849808394949ULL,  2271977612711687670ULL,
   4108833945779251195ULL,  568007465070970865ULL,   12917651905069323917ULL,
   16162436979345134787ULL, 18094670094831889504ULL, 5159635793489146569ULL,
   1055811408795829079ULL,  4059476286632342525ULL,  3136548617057760422ULL,
   15208822057804522650ULL, 16779379672353154414ULL, 16945566572844229360ULL,
   6394431736101640909ULL,  15971733067870382541ULL, 3074685293590132603ULL,
   14994969907171726030ULL, 5054285249655370902ULL,  4366414375565960721ULL,
   18138704300851448093ULL, 18090909209076981007ULL, 2227468796943162191ULL,
   14690613010662650530ULL, 2965137079614874767ULL,  12511942270583430762ULL,
   6056241825827053459ULL,  8967943645795954101ULL,  2557960620660083973ULL,
   8911628266014211229ULL,  1693410756742750329ULL,  1213726659146807337ULL,
   13292976821553093367ULL, 13614502280461197818ULL, 1272986455802402472ULL,
   14166116471448937393ULL, 1150328286153710687ULL,  2635336778931630969ULL,
   13258800454473040239ULL},
  {13955808865119206894ULL, 3768124203251153802ULL,  7767668454178177238ULL,
   12460946455482264667ULL, 2212909910727681501ULL,  10760113910584082011ULL,
   9374791472919361215ULL,  386920245004536964ULL,   8683453171623630070ULL,
   14422139112427733037ULL, 15737503484882653327ULL, 1429520381208348380ULL,
   11789198310227604022ULL, 5346932736182592592ULL,  6992108487598772046ULL,
   3810143436105504444ULL,  9991967600932716204ULL,  18354697977652341999ULL,
   12493530606327658139ULL, 13021759389121594195ULL, 16664146324550766970ULL,
   13224832207684479436ULL, 17720203714796126921ULL, 10589712267341406664ULL,
   675431606122912244ULL,   15436011181826765452ULL, 17419184006903181237ULL,
   15622323875903275509ULL, 12603723962613730547ULL, 8427334651928243865ULL,
   4878436834959292999ULL,  5443734806597558100ULL,  8060589086878591310ULL,
   17390775264282771123ULL, 11689512766269424321ULL, 6919534634806107325ULL,
   7412922511069379105ULL,  1613690698443232454ULL,  8755905037628623177ULL,
   10398559429723932968ULL, 17333529230847651769ULL, 4937282078494887907ULL,
   6705193566984479245ULL,  12430286470210107756ULL, 17074605555638295522ULL,
   6529312161204558858ULL,  13619885933920180546ULL, 16445305908599040110ULL,
   6235639583958465813ULL,  8697364242063900525ULL,  12123766938633741655ULL,
   12773557164416789371ULL, 1409904598267027719ULL,  6871406007286507231ULL,
   17752268963604186079ULL, 10339626746932065374ULL, 16451186088466478926ULL,
   11515214132125761651ULL, 11900866616174196547ULL, 4054127626110162050ULL,
   2813729167263171469ULL,  8593992021203111351ULL,  7514147282072964872ULL,
   4565556059429381617ULL},
  {14899878500819448805ULL, 11092772676875200070ULL, 3011563621789303869ULL,
   8431736601431563477ULL,  11094056159060029407ULL, 5472679836209143101ULL,
   5964456097387628717ULL,  12172487332366350462ULL, 5870338891818055ULL,
   8362862985679364348ULL,  3679416165959319381ULL,  7492280119574977853ULL,
   14551216335097803542ULL, 18079686034935279961ULL, 5222451405613384284ULL,
   1592757622938144582ULL,  964833792474744395ULL,   4117838928961454628ULL,
   15624002789203860172ULL, 4873068055253463122ULL,  18035015145881423744ULL,
   2290860586257805917ULL,  3105824466576806351ULL,  8478484625958259410ULL,
   12491768566436833807ULL, 8318793226685721696ULL,  5640027455098779414ULL,
   3611731914894190066ULL,  13132357072258725611ULL, 17550664693569430902ULL,
   8040100906467573389ULL,  8943283014271962826ULL,  1165543225903175261ULL,
   8274338419540601119ULL,  14758525360534801665ULL, 12264227083413645728ULL,
   1703338559514121679ULL,  6403781546322027998ULL,  18117498816386664843ULL,
   8964211536133681007ULL,  8836902312038230476ULL,  14157204555053453451ULL,
   767782710556956092ULL,   4534169985317255621ULL,  9456116068111314591ULL,
   16873778622662553850ULL, 16738581987983783289ULL, 3394372497525621191ULL,
   4115861130318239651ULL,  1482942667955471119ULL,  15463552115154337513ULL,
   2636288131138043013ULL,  15717924344822515264ULL, 10140860132569109304ULL,
   13531012961419342048ULL, 1596598458546143014ULL,  16464642794698645037ULL,
   2512324327060157939ULL,  6810523751360712901ULL,  4285239444643791090ULL,
   9169032563330614215ULL,  11131869812112038302ULL, 14040603873421166650ULL,
   16811188507134353559ULL},
  {4647175973998783469ULL,  3670497068185862043ULL,  9121596771722230582ULL,
   7456525605611567042ULL,  11664999355162333109ULL, 4134706980834149414ULL,
   13320331952684352014ULL, 7592144027201669803ULL,  15878188174809315405ULL,
   13969401062719840453ULL, 14374280681323717647ULL, 14386174965380256303ULL,
   12516955245451170241ULL, 8455155087567140425ULL,  3022694100375074004ULL,
   753253462383394001ULL,   9467966939520962792ULL,  15038094624267705573ULL,
   13675651613580794906ULL, 10788542426313748727ULL, 873478227701325053ULL,
   10780247635346219977ULL, 8171964733160946074ULL,  6058973372072967844ULL,
   9087029515372359690ULL,  2570220291576920824ULL,  10575623531745879639ULL,
   893278122153289812ULL,   15375448293095930448ULL, 1915728790809455234ULL,
   18220040615960771312ULL, 4827662770812431581ULL,  12454476905683780984ULL,
   1812522311696963809ULL,  15514364871402399571ULL, 7421548766666439074ULL,
   8165003385105507149ULL,  15131000749384236273ULL, 1153519283671629440ULL,
   5741283157527470809ULL,  12593604742369711390ULL, 307094473243738552ULL,
   5998016196305137768ULL,  11456124341965012040ULL, 819016756549072850ULL,
   1654713130621659827ULL,  47679947860186164ULL,    15668166860936613294ULL,
   1739150790475959808ULL,  17088648099951568335ULL, 5097464977937510436ULL,
   12921506022902466080ULL, 18126966833127810557ULL, 16873947333866178913ULL,
   3573966125358988573ULL,  8120796109217796433ULL,  8702505085715867854ULL,
   9080592210572690011ULL,  6262889957229711933ULL,  6969130906618964678ULL,
   4896578836317062571ULL,  7355939195761273860ULL,  11853167942771954328ULL,
   10254441313474848044ULL},
  {1815953443210061576ULL,  487153716366729054ULL,   1583582953868121480ULL,
   3602427446813989298ULL,  2483607456558983803ULL,  10998014208826511215ULL,
   8436888566373427645ULL,  1115389032719111830ULL,  1366631601253183101ULL,
   4767314847741135679ULL,  18143053023536865735ULL, 927908684995703482ULL,
   15737114700043987271ULL, 15212811775721029943ULL, 8172793054981155328ULL,
   17057793979855197385ULL, 13497061770400685638ULL, 3850795411923878847ULL,
   6265520288483945226ULL,  14919198037236574780ULL, 15912629001295882723ULL,
   8939400326073801381ULL,  8563362012272863419ULL,  16497389276993918103ULL,
   15038668154919536966ULL, 8576337074522670199ULL,  5093612626764180466ULL,
   15550958953392072654ULL, 7920900775933594532ULL,  4848320143246285205ULL,
   10342977723229100935ULL, 4145145577009213406ULL,  14638195844828979854ULL,
   9485538787733691710ULL,  18023338232557765610ULL, 2030498806765193588ULL,
   7123994810125671502ULL,  16317505238933071387ULL, 11693926166189241488ULL,
   8427943714959616231ULL,  4937898530559007932ULL,  4035286215119511399ULL,
   13712415440060050859ULL, 10912753103198463537ULL, 5801204826565652051ULL,
   18359383048568531456ULL, 1087897188507972888ULL,  13314332990707131491ULL,
   5330341598872539088ULL,  13626584938438709098ULL, 13578141067577849023ULL,
   13879708106949016455ULL, 4981266529621914324ULL,  11430645334823269475ULL,
   17158744742380393406ULL, 11491178647262355167ULL, 10387510382824534290ULL,
   11217384462514426810ULL, 10800288610458713454ULL, 10954004379756349935ULL,
   9970438718019081428ULL,  2993204881762914966ULL,  12480875466975500879ULL,
   16947820389578877186ULL},
  {5765828304650637872ULL,  17893631662867024904ULL, 15953806000662978409ULL,
   6859943130409225073ULL,  17515189280069308372ULL, 7675646577614782550ULL,
   17627126536248415982ULL, 5417562177564309588ULL,  14984121188552189497ULL,
   17615324481473371334ULL, 12858191362294733946ULL, 12571782197145243393ULL,
   4034722490447671488ULL,  10472942492761989254ULL, 779573933699361860ULL,
   3267735036985233538ULL,  10039021968881901456ULL, 13406683324781001594ULL,
   12366195917680709420ULL, 18087851237281379737ULL, 5440403459438360416ULL,
   3768469965793332370ULL,  2766210920635948181ULL,  11781205728811476291ULL,
   7768235191197925629ULL,  63633402547340130ULL,    5367245296745327549ULL,
   9330461602658406982ULL,  1599782758386869348ULL,  11902971525634483069ULL,
   2348828175157619938ULL,  17129658353585995592ULL, 134502031972416144ULL,
   14598547107262421501ULL, 4826643625087548859ULL,  13701423684794803964ULL,
   15331184009377593578ULL, 2218382000173997389ULL,  5119849419857239673ULL,
   9905990754695968495ULL,  11041068110782960018ULL, 10721421337025430524ULL,
   18386599558831479036ULL, 8354739657820236341ULL,  10416359819798378908ULL,
   13346048107481237903ULL, 361428534470795476ULL,   16553361915532000798ULL,
   8880853381586629287ULL,  2147635570103667359ULL,  7902404345018495858ULL,
   3411286335056327075ULL,  4341900803786390089ULL,  15505958031553495526ULL,
   16775869251575674615ULL, 15600866811886767005ULL, 6168738515015117310ULL,
   8722129436221509160ULL,  13767295096142081804ULL, 14155530168968463083ULL,
   17991836959946654653ULL, 9658122390926781514ULL,  4849739298385429147ULL,
   1848096582114946003ULL},
  {15630960698809199186ULL, 14554810390679960010ULL, 6663109145389600661ULL,
   9919642392925345966ULL,  4433935500724629923ULL,  9697764300709961210ULL,
   13778028397947264642ULL, 14154593721163773976ULL, 8934579092702244501ULL,
   18310913279972021423ULL, 13596188488443984460ULL, 17453184401801637286ULL,
   1292716026805305512ULL,  16885868385984897276ULL, 180539353689186535ULL,
   6507065998173897883ULL,  8588697330211782889ULL,  5178631872466346094ULL,
   2143016449841078418ULL,  2195454733455258051ULL,  10650525108596009238ULL,
   16753111911229024957ULL, 1068600790087848949ULL,  12021047953800045212ULL,
   12842582036983941753ULL, 496313128426624146ULL,   15958994691786328826ULL,
   17428720547520281443ULL, 6991283341228096884ULL,  768167946099390407ULL,
   1018590435363284668ULL,  7950611884970745065ULL,  13036070309279674443ULL,
   12918399057793253091ULL, 1742838079779998946ULL,  4169737234259971530ULL,
   17574089608367116030ULL, 617654094587802950ULL,   1550858333420650933ULL,
   752416328929302830ULL,   9690335958110697184ULL,  4330154216326281194ULL,
   26860376667680997ULL,    858658967571861456ULL,   10910582069694292392ULL,
   3154633618975749178ULL,  4597083263635304163ULL,  17659761952422734669ULL,
   17739697585626342906ULL, 3620406158586983895ULL,  15096273928312548608ULL,
   2795057506438515921ULL,  4350077675904100122ULL,  16947641656103836430ULL,
   2180783775224466986ULL,  12182365965819268546ULL, 9522248171988432732ULL,
   18322950849316981753ULL, 6312990439576278417ULL,  17423079996007997796ULL,
   3524745975529248856ULL,  3756548955483732063ULL,  1136829687742208963ULL,
   18164582564071484163ULL},
  {337636894567421668ULL,   5745244704136977085ULL,  10438267066191668813ULL,
   10745016807913477313ULL, 1520958538440147131ULL,  17054000131081311960ULL,
   17877429922411352085ULL, 5459272206022843180ULL,  17160913026759013125ULL,
   8777663329984338907ULL,  7317894943553933623ULL,  969866752853870131ULL,
   1779524316557042444ULL,  3871948692386157470ULL,  1040003253814120632ULL,
   3948662290683762118ULL,  16489069782923166298ULL, 8937880741731937694ULL,
   14696905903797791277ULL, 6044511698456694600ULL,  4050328584041511431ULL,
   6386120570538996155ULL,  6156633081721439591ULL,  12297394843421120227ULL,
   15577847232942588311ULL, 8525816218507955556ULL,  8129302999183972534ULL,
   17795994492286375090ULL, 2078242541614810717ULL,  6960819876442009100ULL,
   7812412049993327018ULL,  8181496981638525805ULL,  17558781786350643761ULL,
   2634355373798617682ULL,  16595808928698627324ULL, 11366139777004566376ULL,
   2624832078201433940ULL,  10421379751600233615ULL, 4727439935412568615ULL,
   339219732470399687ULL,   15502131885505979012ULL, 18395808927094082082ULL,
   12060184286878980066ULL, 12594118147485736712ULL, 17662665058501306450ULL,
   3574063932168969713ULL,  10281295385618513288ULL, 12055671337766144243ULL,
   2558251103697603861ULL,  15438272733531031189ULL, 4572431571709843508ULL,
   16480481191164951233ULL, 1802930964370506174ULL,  917212612807223679ULL,
   10049360639731978831ULL, 16260920641251793876ULL, 7578018926274969931ULL,
   8619193919401821768ULL,  8299426002727893385ULL,  16002473735705465297ULL,
   6906640411307036588ULL,  12519881132829017655ULL, 13406238921811916915ULL,
   15287733648752578209ULL},
  {14410666874905257339ULL, 6443039859299686139ULL,  17116844314575655100ULL,
   8914603156101374394ULL,  11635564710329290074ULL, 729041918289623496ULL,
   17955077722898381736ULL, 2489241484782758699ULL,  15535520537309762171ULL,
   8007812918135346562ULL,  8162506532178751702ULL,  1900362042974283048ULL,
   6127427286118616482ULL,  5203836461443263763ULL,  10182382520846945476ULL,
   2914725446488327421ULL,  12794935148206698288ULL, 10890966252350731835ULL,
   3056292900927865735ULL,  106280882535446496ULL,   4926102836303436224ULL,
   2676156109274474978ULL,  16276829956552571003ULL, 5076341141574264843ULL,
   11258104302454591761ULL, 6078662789346954333ULL,  11002921186404590367ULL,
   9803435889868858137ULL,  1164677890309912711ULL,  10885375524046208769ULL,
   11388535049105870843ULL, 3702194756440921639ULL,  16529922861652421758ULL,
   10407504625737701804ULL, 12552269097897872688ULL, 3456999539676058994ULL,
   2386691024812600312ULL,  1878571581486268832ULL,  2485613075999504031ULL,
   1108772379220667622ULL,  9155718661709713993ULL,  2510836544777560910ULL,
   3752196244729536131ULL,  17475732340895300029ULL, 1333052936498244100ULL,
   16452548121947858805ULL, 1256776841797929461ULL,  16105080276140519587ULL,
   8359725079869541046ULL,  5106806381743785602ULL,  6057137068070893292ULL,
   4756388830004672812ULL,  3416361119195527208ULL,  13197862245020958795ULL,
   670700304478626650ULL,   4479000566239547858ULL,  1506460954441485241ULL,
   4297197770920434372ULL,  3845800095180131401ULL,  17330463919046705096ULL,
   13092243888955501788ULL, 11602111107737187444ULL, 3654639935551267922ULL,
   2570110211549736030ULL},
  {8070307556367204911ULL,  15167282530121961166ULL, 9165282866122119232ULL,
   2538800935920404327ULL,  776803568794996619ULL,   12459692628927896092ULL,
   6880981412764564566ULL,  2096545373494757616ULL,  11163142062222298281ULL,
   11187835505382040919ULL, 12181284323532048280ULL, 2701964775298600929ULL,
   6922744823339659558ULL,  18348304233929620256ULL, 6246271807990195832ULL,
   9920449296221083036ULL,  1413256321128499017ULL,  9417485021217070322ULL,
   15966859415068201410ULL, 8019753950986999440ULL,  8744019072379432655ULL,
   2274657166606465539ULL,  7607452605379762920ULL,  17575045238031263553ULL,
   11295094036697134147ULL, 14893663217019927176ULL, 8239402451285919624ULL,
   7416459469103768820ULL,  17188501023137760829ULL, 14162213669426765312ULL,
   778832167771747567ULL,   7671687585205876718ULL,  10756174218786091353ULL,
   9754332132768801534ULL,  2741378732246662660ULL,  2001291864375202750ULL,
   10877323946916717010ULL, 13417008588885550779ULL, 12072137353223237491ULL,
   17394488350074758130ULL, 10139494526358007999ULL, 9305420268849200352ULL,
   18392182415293459118ULL, 1782626638397053770ULL,  12171733466606470265ULL,
   16439073861482739367ULL, 16868781108399019585ULL, 4954871356512072470ULL,
   10562742869996290147ULL, 5969636146788474769ULL,  1127549119144574041ULL,
   16580092125405552416ULL, 12343381126468533984ULL, 12435959365892883551ULL,
   1132216750958082069ULL,  14850373427016934059ULL, 7981565447673735122ULL,
   3854286082503446368ULL,  13826152773361452644ULL, 4865132865926611438ULL,
   10306579347202345843ULL, 15976468767339030698ULL, 14181943498817654479ULL,
   8352544142070819244ULL}};

// Little-Endian Rank-File Mapping
// clang-format off
static const uint8_t CASTLE_RIGHTS_MODIFIERS[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};
// clang-format on

Position::Position() {
    this->board            = Board();
    this->active_color     = WHITE;
    this->casteling_rights = NOCA;
    this->enpassant_target = NO_SQ;
    this->half_move_clock  = 0;
    this->full_move_number = 0;
    this->ply_count        = 0;
    this->hash             = 0ULL;
}

Position::Position(const Position& position) {
    this->board            = position.board;  // TODO: Optimize
    this->active_color     = position.active_color;
    this->casteling_rights = position.casteling_rights;
    this->enpassant_target = position.enpassant_target;
    this->half_move_clock  = position.half_move_clock;
    this->full_move_number = position.full_move_number;
    this->ply_count        = position.ply_count;
    this->hash             = position.hash;
}

void Position::set_piece(const Piece piece, const Square square) {
    this->board.set_piece(piece, square);
    this->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}
void Position::clear_piece(const Piece piece, const Square square) {
    this->board.clear_piece(piece, square);
    this->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}

void Position::set_active_color(const Color active_color) { this->active_color = active_color; }

void Position::add_casteling_rights(const CastleFlag castle_flag) {
    this->casteling_rights |= castle_flag;
}
void Position::set_enpassant_target(const Square enpassant_target) {
    this->enpassant_target = enpassant_target;
}
void Position::set_half_move_clock(const uint32_t half_move_clock) {
    this->half_move_clock = half_move_clock;
}
void Position::set_full_move_number(const uint32_t full_move_number) {
    this->full_move_number = full_move_number;
}

void Position::reset_ply_count() { this->ply_count = 0; }

void Position::reset_hash() {
    this->hash = 0ULL;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        const Piece p = this->board.get_piece(static_cast<Square>(sq));
        if (p == NO_PIECE)
            continue;

        const uint8_t piece_hash_idx = HASH_IDX(p);
        this->hash ^= HASHTABLE[piece_hash_idx][sq];
    }
}

void Position::set_start_pos() {
    this->board.set_start_pos();
    this->active_color     = WHITE;
    this->casteling_rights = WKCA | WQCA | BKCA | BQCA;
    this->enpassant_target = NO_SQ;
    this->half_move_clock  = 0;
    this->full_move_number = 1;
    this->ply_count        = 0;
    this->reset_hash();
}

bool Position::move_do_on_complete() {
    const bool is_legal_move = (!this->is_in_check()) && this->is_valid();
    this->active_color       = static_cast<Color>(this->active_color ^ 1);
    return is_legal_move;
}

bool Position::move_do(const Move move) {
    const Square   move_from_sq = move.get_from();
    const Square   move_to_sq   = move.get_to();
    const Piece    move_piece   = this->get_piece(move_from_sq);
    const MoveFlag move_flag    = move.get_flag();

    if (PIECE_GET_COLOR(move_piece) != this->active_color)
    {
        throw std::invalid_argument("Invalid move!");
    }

    const MoveHistoryEntry mhe(move, this->casteling_rights, this->enpassant_target,
                               this->half_move_clock, this->full_move_number, this->hash);
    this->history.push(mhe);

    this->enpassant_target = NO_SQ;
    this->half_move_clock++;
    if (this->active_color == BLACK)
        this->full_move_number++;
    this->ply_count++;

    this->clear_piece(move_piece, move_from_sq);

    if (move_flag == MOVE_QUIET_PAWN_DBL_PUSH)
    {
        const int8_t side_to_move = 1 - (2 * this->active_color);  // WHITE = 1; BLACK = -1
        this->enpassant_target    = static_cast<Square>(move_from_sq + (8 * side_to_move));
        this->set_piece(move_piece, move_to_sq);
        this->half_move_clock = 0;
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CAPTURE_EP)
    {
        const int8_t side_to_move        = -1 + (2 * this->active_color);  // WHITE = -1; BLACK = 1
        const Square ep_captured_pawn_sq = static_cast<Square>(move_to_sq + (8 * side_to_move));
        const Piece  ep_captured_pawn =
          PIECE_CREATE(PAWN, static_cast<Color>(this->active_color ^ 1));
        this->clear_piece(ep_captured_pawn, ep_captured_pawn_sq);
        this->set_piece(move_piece, move_to_sq);
        this->half_move_clock = 0;
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CASTLE_KING_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, this->active_color);
        const Piece rook = PIECE_CREATE(ROOK, this->active_color);
        this->set_piece(king, move_to_sq);
        this->clear_piece(rook, static_cast<Square>(move_to_sq + 1));
        this->set_piece(rook, static_cast<Square>(move_to_sq - 1));
        this->casteling_rights &= CASTLE_RIGHTS_MODIFIERS[move_from_sq];
        return this->move_do_on_complete();
    }

    if (move_flag == MOVE_CASTLE_QUEEN_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, this->active_color);
        const Piece rook = PIECE_CREATE(ROOK, this->active_color);
        this->set_piece(king, move_to_sq);
        this->clear_piece(rook, static_cast<Square>(move_to_sq - 2));
        this->set_piece(rook, static_cast<Square>(move_to_sq + 1));
        this->casteling_rights &= CASTLE_RIGHTS_MODIFIERS[move_from_sq];
        return this->move_do_on_complete();
    }

    if (MOVE_IS_CAPTURE(move_flag))
    {
        this->clear_piece(this->board.get_piece(move_to_sq), move_to_sq);
        this->half_move_clock = 0;
    }

    if (MOVE_IS_PROMOTION(move_flag))
    {
        switch (move_flag)
        {
            case MOVE_PROMOTION_KNIGHT :
            case MOVE_CAPTURE_PROMOTION_KNIGHT :
                this->set_piece(PIECE_CREATE(KNIGHT, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_BISHOP :
            case MOVE_CAPTURE_PROMOTION_BISHOP :
                this->set_piece(PIECE_CREATE(BISHOP, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_ROOK :
            case MOVE_CAPTURE_PROMOTION_ROOK :
                this->set_piece(PIECE_CREATE(ROOK, this->active_color), move_to_sq);
                break;
            case MOVE_PROMOTION_QUEEN :
            case MOVE_CAPTURE_PROMOTION_QUEEN :
                this->set_piece(PIECE_CREATE(QUEEN, this->active_color), move_to_sq);
                break;
            default :
                break;
        }
    }
    else
    {
        this->set_piece(move_piece, move_to_sq);
    }

    if (PIECE_GET_TYPE(move_piece) == PAWN)
        this->half_move_clock = 0;

    this->casteling_rights &=
      (CASTLE_RIGHTS_MODIFIERS[move_from_sq] & CASTLE_RIGHTS_MODIFIERS[move_to_sq]);

    return this->move_do_on_complete();
}

bool Position::move_do(const std::string move_str) {
    if (move_str.length() < 4 || move_str.length() > 5)
        throw std::invalid_argument("Invalid move string!");

    const File   from_file = static_cast<File>(move_str[0] - 'a');
    const Rank   from_rank = static_cast<Rank>((move_str[1] - '0') - 1);
    const File   to_file   = static_cast<File>(move_str[2] - 'a');
    const Rank   to_rank   = static_cast<Rank>((move_str[3] - '0') - 1);
    const Square from_sq   = BOARD_RF_TO_SQ(from_rank, from_file);
    const Square to_sq     = BOARD_RF_TO_SQ(to_rank, to_file);

    Piece move_piece     = this->get_piece(from_sq);
    Piece captured_piece = this->get_piece(to_sq);

    const Rank pawn_promotion_rank = static_cast<Rank>(7 * (this->active_color ^ 1));
    const bool is_promotion_move   = (move_str.length() == 5) && (to_rank == pawn_promotion_rank);

    MoveFlag flag;

    if (captured_piece == NO_PIECE)
    {
        if (PIECE_GET_TYPE(move_piece) == PAWN)
        {
            if (this->active_color == WHITE && from_rank == RANK_2 && to_rank == RANK_4)
                flag = MOVE_QUIET_PAWN_DBL_PUSH;
            else if (this->active_color == BLACK && from_rank == RANK_7 && to_rank == RANK_5)
                flag = MOVE_QUIET_PAWN_DBL_PUSH;
            else if (is_promotion_move)
            {
                switch (move_str[4])
                {
                    case 'n' :
                        flag = MOVE_PROMOTION_KNIGHT;
                        break;
                    case 'b' :
                        flag = MOVE_PROMOTION_BISHOP;
                        break;
                    case 'r' :
                        flag = MOVE_PROMOTION_ROOK;
                        break;
                    case 'q' :
                        flag = MOVE_PROMOTION_QUEEN;
                        break;
                    default :
                        flag = MOVE_PROMOTION_QUEEN;
                        break;
                }
            }
            else
                flag = MOVE_QUIET;
        }
        else if (PIECE_GET_TYPE(move_piece) == KING)
        {
            if (this->active_color == WHITE)
            {
                if ((this->casteling_rights & WKCA) && from_sq == E1 && to_sq == G1)
                    flag = MOVE_CASTLE_KING_SIDE;
                else if ((this->casteling_rights & WQCA) && from_sq == E1 && to_sq == C1)
                    flag = MOVE_CASTLE_QUEEN_SIDE;
                else
                    flag = MOVE_QUIET;
            }
            else
            {
                if ((this->casteling_rights & BKCA) && from_sq == E8 && to_sq == G8)
                    flag = MOVE_CASTLE_KING_SIDE;
                else if ((this->casteling_rights & BQCA) && from_sq == E8 && to_sq == C8)
                    flag = MOVE_CASTLE_QUEEN_SIDE;
                else
                    flag = MOVE_QUIET;
            }
        }
        else
            flag = MOVE_QUIET;
    }
    else
    {
        if (PIECE_GET_TYPE(move_piece) == PAWN)
        {
            if (is_promotion_move)
            {
                switch (move_str[4])
                {
                    case 'n' :
                        flag = MOVE_CAPTURE_PROMOTION_KNIGHT;
                        break;
                    case 'b' :
                        flag = MOVE_CAPTURE_PROMOTION_BISHOP;
                        break;
                    case 'r' :
                        flag = MOVE_CAPTURE_PROMOTION_ROOK;
                        break;
                    case 'q' :
                        flag = MOVE_CAPTURE_PROMOTION_QUEEN;
                        break;
                    default :
                        flag = MOVE_CAPTURE_PROMOTION_QUEEN;
                        break;
                }
            }
            else if (to_sq == this->enpassant_target)
                flag = MOVE_CAPTURE_EP;
            else
                flag = MOVE_CAPTURE;
        }
        else
            flag = MOVE_CAPTURE;
    }

    const unsigned int move_score =
      captured_piece == NO_PIECE ? 0 : MOVE_SCORE_MVV_LVA_IDX(move_piece, captured_piece);

    const Move move(from_sq, to_sq, flag, captured_piece, move_score);

    return this->move_do(move);
}

void Position::move_undo() {
    const MoveHistoryEntry& mhe               = this->history.top();
    const Move              prev_move         = mhe.move;
    const Square            prev_move_from_sq = prev_move.get_from();
    const Square            prev_move_to_sq   = prev_move.get_to();
    const MoveFlag          prev_move_flag    = prev_move.get_flag();
    const Color             prev_active_color = static_cast<Color>(this->active_color ^ 1);

    if (prev_move_flag == MOVE_CASTLE_KING_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, prev_active_color);
        const Piece rook = PIECE_CREATE(ROOK, prev_active_color);
        this->clear_piece(king, prev_move_to_sq);
        this->set_piece(king, prev_move_from_sq);
        this->clear_piece(rook, static_cast<Square>(prev_move_to_sq - 1));
        this->set_piece(rook, static_cast<Square>(prev_move_to_sq + 1));
    }
    else if (prev_move_flag == MOVE_CASTLE_QUEEN_SIDE)
    {
        const Piece king = PIECE_CREATE(KING, prev_active_color);
        const Piece rook = PIECE_CREATE(ROOK, prev_active_color);
        this->clear_piece(king, prev_move_to_sq);
        this->set_piece(king, prev_move_from_sq);
        this->clear_piece(rook, static_cast<Square>(prev_move_to_sq + 1));
        this->set_piece(rook, static_cast<Square>(prev_move_to_sq - 2));
    }
    else if (prev_move_flag == MOVE_CAPTURE_EP)
    {
        const Piece pawn = PIECE_CREATE(PAWN, prev_active_color);
        this->clear_piece(pawn, prev_move_to_sq);
        this->set_piece(pawn, prev_move_from_sq);

        const Piece  ep_captured_pawn = PIECE_CREATE(PAWN, this->active_color);
        const int8_t side_to_move     = 1 - (2 * this->active_color);  // WHITE = 1; BLACK = -1
        const Square ep_captured_pawn_sq =
          static_cast<Square>(prev_move_to_sq + (8 * side_to_move));
        this->set_piece(ep_captured_pawn, ep_captured_pawn_sq);
    }
    else if (MOVE_IS_PROMOTION(prev_move_flag))
    {
        const Piece promoted_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(promoted_piece, prev_move_to_sq);
        this->set_piece(PIECE_CREATE(PAWN, prev_active_color), prev_move_from_sq);
        if (MOVE_IS_CAPTURE(prev_move_flag))
            this->set_piece(prev_move.get_captured(), prev_move_to_sq);
    }
    else if (MOVE_IS_CAPTURE(prev_move_flag))
    {
        Piece prev_move_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(prev_move_piece, prev_move_to_sq);
        this->set_piece(prev_move.get_captured(), prev_move_to_sq);
        this->set_piece(prev_move_piece, prev_move_from_sq);
    }
    else
    {
        Piece prev_move_piece = this->board.get_piece(prev_move_to_sq);
        this->clear_piece(prev_move_piece, prev_move_to_sq);
        this->set_piece(prev_move_piece, prev_move_from_sq);
    }

    assert(this->hash == mhe.prev_hash);
    assert((this->active_color ^ 1) == prev_active_color);

    this->active_color     = prev_active_color;
    this->casteling_rights = mhe.prev_casteling_rights;
    this->enpassant_target = mhe.prev_enpassant_target;
    this->half_move_clock  = mhe.prev_half_move_clock;
    this->full_move_number = mhe.prev_full_move_number;
    this->ply_count--;

    this->history.pop();
}

Piece Position::get_piece(const Square square) const { return this->board.get_piece(square); }

uint8_t Position::get_piece_count(const Piece piece) const {
    return this->board.get_piece_count(piece);
}

Color Position::get_active_color() const { return this->active_color; }

uint32_t Position::get_half_move_clock() const { return this->half_move_clock; }

uint32_t Position::get_ply_count() const { return this->ply_count; }

uint64_t Position::get_hash() const { return this->hash; }

bool Position::is_valid() const { return this->board.is_valid(); }

bool Position::is_in_check() const { return this->board.is_in_check(this->active_color); }

bool Position::is_repeated() const {
    for (uint16_t i = (this->ply_count - this->half_move_clock); i < this->ply_count - 1; ++i)
    {
        if (this->hash == this->history.peek(i).prev_hash)
            return true;
    }
    return false;
}

void Position::generate_pseudolegal_moves(ArrayList<Move>* move_list,
                                          const bool       only_captures) const {
    this->board.generate_pseudolegal_moves(this->active_color, this->casteling_rights,
                                           this->enpassant_target, only_captures, move_list);
}

void Position::display() const {
    this->board.display();
    const std::string color_str = "wb";
    printf("\n%c %d %d %d %d %d\n%" PRIu64 "\n\n", color_str[this->active_color],
           this->casteling_rights, this->enpassant_target, this->half_move_clock,
           this->full_move_number, this->ply_count, this->hash);
}
