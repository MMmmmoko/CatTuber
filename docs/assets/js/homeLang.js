const path = window.location.pathname;
const parts = path.split("/").filter(Boolean);


console.log("homelang start");

const translations = {
    
  zh: { 
  title: "萌趣角色，让你的输入跃然屏上",
  introduction: "可爱角色生动呈现你的每一次按键、点击与操控， <br> 为桌面或直播间增添灵动色彩.",
  feature1: "键鼠、游戏手柄、音游手台",
  feature2: "双机直播",
  feature3: "创意工坊",
  feature4: "内置/外置面捕",
},
  en: { 
    title: "Showcase Every Input with Adorable Characters", 
    introduction: "Display every keystroke, click, and button press on screen with lively character animations.<br>A charming input overlay for your desktop and livestream.", 
  feature1: "Keyboard & Mouse, Game Controller, Rhythm Game Controller",
  feature2: "Dual-PC Streaming",
  feature3: "Workshop",
  feature4: "Built-in / External Face Tracking",
},
};




if(parts.length>0&&parts[0]=="CatTuberWiki")
{

  parts.shift();
}
let  lang=(parts.length > 0 && translations[parts[0]] )? parts[0] : "zh";




document.querySelectorAll("[data-ctblc]").forEach(el => {
  const key = el.getAttribute("data-ctblc");
      if (translations[lang] && translations[lang][key]) {
      el.innerHTML  = translations[lang][key];
      console.log(translations[lang][key]);
    }
});