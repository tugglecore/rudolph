// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';
import starlightThemeGalaxy from 'starlight-theme-galaxy'

// https://astro.build/config
export default defineConfig({
	integrations: [
		starlight({
			title: 'Rudolph',
			social: [{ icon: 'github', label: 'GitHub', href: 'https://github.com/withastro/starlight' }],
            plugins: [starlightThemeGalaxy()],
            sidebar: [
                {
                    label: 'Introduction',
                    autogenerate: { directory: 'introduction' }
                },
                {
                    label: 'Reference',
                    autogenerate: { directory: 'reference' }
                },
                {
                    label: 'Resources',
                    autogenerate: { directory: 'resources' }
                }
            ]
		}),
	],
});

//#0070c0 
