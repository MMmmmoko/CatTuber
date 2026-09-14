const path = window.location.pathname;
const parts = path.split("/").filter(Boolean);

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



const lang=parts.length > 0 && translations[parts[0]] ? parts[0] : "en";
// 开发用
if(patrs[0]=="g%3A")lang="zh";


document.querySelectorAll("[data-ctblc]").forEach(el => {
  const key = el.getAttribute("data-ctblc");
      if (translations[lang] && translations[lang][key]) {
      el.innerHTML  = translations[lang][key];
    //   console.log(translations[lang][key]);
    }
});