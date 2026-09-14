---
class: homepage
hide:
  - navigation
  - toc
---

<style>
[data-md-color-scheme="default"] .md-main {
    background-color: transparent;
    background-image: none;
}


[data-md-color-scheme="slate"] .md-main {
    background-color: transparent;
    background-image: none;
}

body{
margin: 0;
}

.md-content{
    margin: 0;         /* 去掉居中 */
  padding: 0  !important;        /* 可选，去掉默认内边距 */
    }

.md-main__inner.md-grid {
  flex: 1;
     max-width: none;
    margin: 0;         /* 去掉居中 */
  padding: 0;        /* 可选，去掉默认内边距 */
    }

.md-content__inner {
    margin: 0;         /* 去掉居中 */
  padding: 0;        /* 可选，去掉默认内边距 */
    }

.md-content__inner.md-typeset::before
{
  height: 0;
}




.ctb-check-box {
  display: flex;
  align-items: center;
  gap: 0.5rem; /* 图标和文字间距 */
  margin-top: 0.5rem;
}
.ctb-check-btn {
  border: none;
  border-radius: 9999px; /* 圆角矩形 */
  background: linear-gradient(to right, #f282d7, #aa90f9, #63d0fc); /* 品红→紫→蓝渐变 */

  color: white;
  width: 1rem  !important;
  height: 1rem  !important;
}

.ctb-check-btn-content{
  width: 1rem  !important;
  height: 1rem  !important;
  fill: white;
}

.steam-btn {
  position:relative;
  display: flex;
  justify-content: center;   /* 水平方向居中 */
  align-items: center;
  gap: 0.04rem; /* 图标和文字间距 */
  padding: 0.5rem 1.2rem 0.5rem 1rem;
  border: none;
  border-radius: 9999px; /* 圆角矩形 */
  background: linear-gradient(to right, #f282d7, #aa90f9, #63d0fc); /* 品红→紫→蓝渐变 */

  color: white;
  font-size: 1rem;
  cursor: pointer;
  /* transition: transform 0.2s ease; */
  width: 8rem;
  height: 2.75rem;
}
/* 白色叠加层 */
.steam-btn::after {
  content: "";
  position: absolute;
  inset: 0; /* 覆盖整个按钮 */
  background: rgba(255, 255, 255, 0.3); /* 半透明白色 */
  opacity: 0;
  transition: opacity 0.3s ease;
  border-radius: inherit; /* 保持圆角一致 */
}
.steam-btn:hover::after {
  opacity: 1;
}
.steam-btn span {
  font-size: 1rem; /* 设置字体大小 */
  font-weight: bold; /* 可选：加粗 */
}
.steam-icon {
  width: 1.8rem;
  height: 1.8rem;
  fill: white; /* 可以直接改颜色 */
}

.hboxContainer {
  display: flex;

  justify-content: center;   /* 水平居中 */
  align-items: center;       /* 垂直居中（可选） */
  flex-wrap: wrap;           /* 超出时自动换行 */
  /* 等距分布 */
  /* justify-content: space-between;  */
  margin-top: 0.5rem;
}

.hboxitem {
   display: flex;
  flex-direction: column; /* 垂直排列：图标 → 简称 → 简介 */
  align-items: center;    /* 水平居中所有子元素 */

  width: 15rem;
  /* background: lightblue; */
}

.feature-icon {
  width: 10rem;
  height: 10rem;
  border: none;
  border-radius: 0.8rem;
  text-align: center;
  margin: 4px 0;
}

.feature-title {
  font-size: 1rem;
  font-weight: bold!important;;
  text-align: center;
  margin: 0.2rem 0 0.4rem 0!important;
}

.feature-desc {
  font-size: 0.8rem;
  text-align: center;
  margin: 0;
  padding: 0;
}






</style>

<div style="flex: 1; ">



<svg style="display: none;">
  <symbol id="icon-check" viewBox="0 0 24 24">
    <path d="M9.9997 15.1709L19.1921 5.97852L20.6063 7.39273L9.9997 17.9993L3.63574 11.6354L5.04996 10.2212L9.9997 15.1709Z"></path>
  </symbol>
</svg>

  <div class="ctb-brief" style="padding-left: 10vw;padding-right: 10vw; padding-top: 10vh;background-color: #fbfcfd; ">
    <a href="index.html" title="{{ config.site_name | e }}">
      <img src="/assets/CatTuberLogo.svg" alt="logo" style="width: 24rem;">
    </a>

    <h1 style="margin-top: 1rem;" > <strong data-ctblc="title">萌趣角色，让你的输入跃然屏上</strong></h1>
    <p data-ctblc="introduction">可爱角色生动呈现你的每一次按键、点击与操控， <br>
      为桌面或直播间增添灵动色彩.</p>
    


<div class="ctb-check-box" style="gap:0.5REM 2rem;flex-wrap: wrap;">
<div class="ctb-check-box">
  <svg class="ctb-check-btn ctb-check-btn-content"><use href="#icon-check"></use></svg>
  <span data-ctblc="feature1">键鼠、游戏手柄、手台</span>
</div>

<div class="ctb-check-box">
  <svg class="ctb-check-btn ctb-check-btn-content"><use href="#icon-check"></use></svg>
  <span data-ctblc="feature2">双机直播</span>
</div>

<div class="ctb-check-box">
  <svg class="ctb-check-btn ctb-check-btn-content"><use href="#icon-check"></use></svg>
  <span data-ctblc="feature3">创意工坊</span>
</div>

<div class="ctb-check-box">
  <svg class="ctb-check-btn ctb-check-btn-content"><use href="#icon-check"></use></svg>
  <span data-ctblc="feature4">内置/外置面捕</span>
</div>
</div>




   <button class="steam-btn" style="margin-top: 32px;" >
    <svg class="steam-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor"><path d="M12.0052 2C6.75435 2 2.44852 6.05 2.04102 11.1975L7.40102 13.4125C7.85518 13.1033 8.40352 12.9208 8.99435 12.9208C9.04685 12.9208 9.09852 12.9242 9.15102 12.9258L11.5352 9.47417V9.425C11.5352 7.34583 13.2252 5.655 15.3052 5.655C17.3835 5.655 19.0752 7.3475 19.0752 9.4275C19.0752 11.5075 17.3835 13.1983 15.3052 13.1983H15.2177L11.821 15.6242C11.821 15.6675 11.8243 15.7117 11.8243 15.7567C11.8243 17.3192 10.5618 18.5867 8.99935 18.5867C7.63685 18.5867 6.48602 17.6092 6.22352 16.3142L2.38602 14.725C3.57435 18.9225 7.42768 22 12.0052 22C17.5277 22 22.0043 17.5225 22.0043 12C22.0043 6.4775 17.5268 2 12.0052 2ZM7.07852 16.6667C7.29685 17.1192 7.67352 17.4992 8.17352 17.7083C9.25435 18.1575 10.501 17.645 10.9502 16.5625C11.1693 16.0375 11.1702 15.4633 10.9543 14.9383C10.7385 14.4133 10.3293 14.0042 9.80685 13.7858C9.28685 13.5692 8.73185 13.5783 8.24185 13.7608L9.51102 14.2858C10.3077 14.6192 10.6852 15.5358 10.3518 16.3317C10.021 17.1292 9.10435 17.5067 8.30685 17.175L7.07852 16.6667ZM17.8185 9.4225C17.8185 8.0375 16.691 6.91 15.306 6.91C13.9185 6.91 12.7935 8.0375 12.7935 9.4225C12.7935 10.81 13.9185 11.935 15.306 11.935C16.6918 11.935 17.8185 10.81 17.8185 9.4225ZM15.3118 7.53C16.3527 7.53 17.2002 8.375 17.2002 9.41833C17.2002 10.4608 16.3527 11.3058 15.3118 11.3058C14.2677 11.3058 13.4243 10.4608 13.4243 9.41833C13.4243 8.375 14.2685 7.53 15.3118 7.53Z"></path></svg>
    <span>Steam</span>
  </button>
  </div>


<div class="waveCanvas" id="waveCanvas" data-wavefrontcolor="blue" data-wavebackcolor="green" style="background-color: #fbfcfd;">
    <!-- <script src="/assets/js/waveAnimation.js" type="module"></script> -->
</div>

<!-- 受不了了，图标憋不出来
<div class="hboxContainer">
  <div class="hboxContainer">
    <div class="hboxitem">
      <img src="/assets/icon/homePage_input.svg" class="feature-icon">
      </img>

      <h3 class="feature-title">多元输入</h3>
      <p class="feature-desc">支持多种品牌的游戏手柄<br>支持对无法识别的手柄进行映射</p>

    </div>
    <div class="hboxitem">
      <div class="feature-icon" style="background-color: #fee8f3;  ">
      </div>
      <h3 class="feature-title">双机支持</h3>
      <p class="feature-desc">支持将设备输入转发到推流电脑,<br>直播支持无死角</p>
    </div>
  </div>
  <div class="hboxContainer">
    <div class="hboxitem">
      <div class="feature-icon" style="background-color: #eee3fd;  ">
      </div>
      <h3 class="feature-title">外置面捕</h3>
      <p class="feature-desc">支持接收来自移动设备的面捕数据,<br>电脑压力Down Down</p>
    </div>
    <div class="hboxitem">
      <div class="feature-icon" style="background-color: #ddf7f2;  ">
      </div>
      <h3 class="feature-title">多元模型</h3>
      <p class="feature-desc">支持Spine与Live2D模型，<br>DIY上手无压力！</p>

    </div>
  </div>
</div>
 -->



  
</div>
<script>
  
console.log("CatTuber Wiki Running On Github0" );
console.log("window.location.hostname" +window.location.hostname);
  const isGitHub = window.location.hostname.endsWith("github.io");
  let prefix="";
console.log("CatTuber Wiki Running On Github1" );
  if(isGitHub)
{
  
console.log("CatTuber Wiki Running On Github2" );
  // prefix = window.location.hostname.endsWith("github.io")
  //   ? "/" + window.location.pathname.split("/").filter(Boolean)[0] + "/"
  //   : "/";
  prefix = window.location.hostname.endsWith("github.io")
    ? "/" + window.location.pathname.split("/").filter(Boolean)[0] 
    : "";

// 遍历所有带 src 属性的元素
document.querySelectorAll("[src]").forEach(el => {
    const src = el.getAttribute("src");
    // 跳过已经是绝对路径的（http://、https://、/开头）
    if (src.startsWith("/assets/") )
    // 拼接新路径
      el.src = prefix + src;
console.log(el.src);
});
}

{
const script = document.createElement("script");
script.src = prefix+"/assets/js/homeLang.js";
document.head.appendChild(script);
console.log(script.src );
}

{
const script = document.createElement("script");
script.src = prefix+"/assets/js/waveAnimation.js";
document.head.appendChild(script);
console.log(script.src );
}
</script>
<!-- <script src="/assets/js/homeLang.js" type="module"></script> -->














